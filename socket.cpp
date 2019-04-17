#include <drumlin.h>
#include <tao/json.hpp>
using namespace tao;
#include <algorithm>
#include <mutex>
using namespace std;
using namespace boost;
#include "socket.h"
#include "exception.h"
#include "thread.h"
#include "connection.h"
#include "event.h"

namespace drumlin {

template<>
SocketAdapter<asio::ip::tcp>::SocketAdapter(const socket_type *socket):m_socket(const_cast<socket_type*>(socket))
{
}

template<>
void SocketAdapter<asio::ip::tcp>::error(boost::system::error_code ec)
{
    if(m_socket->m_handler)
        m_socket->m_handler->error(m_socket,ec);
}

template <>
void Socket<asio::ip::tcp>::getStatus(json::value &status)
{
    json::object_t &obj(status.get_object());
    obj.insert({"available",socket().available()});
    obj.insert({"buffered",bytesToWrite()});
}

template<>
SocketAdapter<asio::ip::udp>::SocketAdapter(const socket_type *socket):m_socket(const_cast<socket_type*>(socket))
{
}

template<>
void SocketAdapter<asio::ip::udp>::error(boost::system::error_code ec)
{
    if(m_socket->m_handler)
        m_socket->m_handler->error(m_socket,ec);
}

template<>
void SocketAdapter<asio::ip::tcp>::process()
{
    if(!m_socket->m_handler)
        return;
    make_event(DrumlinEventSocketThreadProcess,"SocketAdapter",this)->send(((ThreadWorker*)m_socket->parent())->getThread());
}

template<>
void SocketAdapter<asio::ip::tcp>::process_impl()
{
    bool replying = false;
    //        if(socketType() == SocketType::TcpSocket) {
    if(m_socket->m_handler->readyProcess(m_socket)){
        Debug() << m_socket << "::processTransmission";
        replying = m_socket->m_handler->processTransmission(m_socket);
    }
    //        }else{
    //            Debug() << this << "::receivePacket";
    //            replying = handler->receivePacket(this);
    //        }
    if(replying){
        Debug() << m_socket << "::reply";
        replying = false;
        bool finished;
        m_socket->setFinished(finished = m_socket->m_handler->reply(m_socket));
        if(finished)
            m_socket->m_handler->completing(m_socket);
    }
}

template<>
void SocketAdapter<asio::ip::tcp>::completing()
{
    if(!m_socket->m_handler)
        return;
    make_event(DrumlinEventSocketThreadCompleting,"SocketAdapter",this)->send(dynamic_cast<ThreadWorker*>(m_socket->parent())->getThread());
}

template<>
void SocketAdapter<asio::ip::tcp>::completing_impl()
{
    m_socket->m_handler->completing(m_socket);
}

template<>
bool SocketAdapter<asio::ip::tcp>::event(Event *pevent)
{
    if((guint32)pevent->type() > (guint32)DrumlinEventSocket_first
            && (guint32)pevent->type() < (guint32)DrumlinEventSocket_last){
        switch(pevent->type()){
        case DrumlinEventSocketThreadProcess:
            process_impl();
            break;
        case DrumlinEventSocketThreadCompleting:
            completing_impl();
            break;
        default:
            return false;
        }
        return true;
    }
    return false;
}

template <>
void Socket<asio::ip::udp>::getStatus(json::value &status)
{
    json::object_t &obj(status.get_object());
    obj.insert({"available",socket().available()});
    obj.insert({"buffered",bytesToWrite()});
}

template<>
void SocketAdapter<asio::ip::udp>::process()
{
    if(!m_socket->m_handler)
        return;
    make_event(DrumlinEventSocketThreadProcess,"SocketAdapter",this)->send(dynamic_cast<ThreadWorker*>(m_socket->parent())->getThread());
}

template<>
void SocketAdapter<asio::ip::udp>::process_impl()
{
    bool replying = false;
    //        if(socketType() == SocketType::TcpSocket) {
    if(m_socket->m_handler->readyProcess(m_socket)){
        Debug() << m_socket << "::processTransmission";
        replying = m_socket->m_handler->processTransmission(m_socket);
    }
    //        }else{
    //            Debug() << this << "::receivePacket";
    //            replying = handler->receivePacket(this);
    //        }
    if(replying){
        Debug() << m_socket << "::reply";
        replying = false;
        bool finished;
        m_socket->setFinished(finished = m_socket->m_handler->reply(m_socket));
        if(finished)
            m_socket->m_handler->completing(m_socket);
    }
}

template<>
void SocketAdapter<asio::ip::udp>::completing()
{
    if(!m_socket->m_handler)
        return;
    make_event(DrumlinEventSocketThreadCompleting,"SocketAdapter",this)->send(dynamic_cast<ThreadWorker*>(m_socket->parent())->getThread());
}

template<>
void SocketAdapter<asio::ip::udp>::completing_impl()
{
    m_socket->m_handler->completing(m_socket);
}

template<>
bool SocketAdapter<asio::ip::udp>::event(Event *pevent)
{
    if((guint32)pevent->type() > (guint32)DrumlinEventSocket_first
            && (guint32)pevent->type() < (guint32)DrumlinEventSocket_last){
        switch(pevent->type()){
        case DrumlinEventSocketThreadProcess:
            process_impl();
            break;
        case DrumlinEventSocketThreadCompleting:
            completing_impl();
            break;
        default:
            return false;
        }
        return true;
    }
    return false;
}

} // namespace drumlin
