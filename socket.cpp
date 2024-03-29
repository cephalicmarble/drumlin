#define TAOJSON
#include "socket.h"

#include <algorithm>
#include <mutex>
using namespace std;
using namespace boost;
#include "drumlin.h"
#include "exception.h"
#include "thread.h"
#include "connection.h"

namespace drumlin {

template<>
SocketAdapter<asio::ip::tcp>::SocketAdapter(Socket<asio::ip::tcp> *socket):m_socket(socket)
{
}

template<>
void SocketAdapter<asio::ip::tcp>::error(boost::system::error_code ec)
{
    if(m_socket->m_connection)
        m_socket->m_connection->error(ec);
    else if(m_socket->handler)
        m_socket->handler->error(m_socket,ec);
}

template <>
void Socket<asio::ip::tcp>::getStatus(json::value &status)
{
    json::object_t &obj(status.get_object());
    obj.insert({"available",socket().available()});
    obj.insert({"buffered",bytesToWrite()});
}

template<>
SocketAdapter<asio::ip::udp>::SocketAdapter(Socket<asio::ip::udp> *socket):m_socket(socket)
{
}

template<>
void SocketAdapter<asio::ip::udp>::error(boost::system::error_code ec)
{
    m_socket->m_connection->error(ec);
}

template <>
void Socket<asio::ip::udp>::getStatus(json::value &status)
{
    json::object_t &obj(status.get_object());
    obj.insert({"available",socket().available()});
    obj.insert({"buffered",bytesToWrite()});
}

} // namespace drumlin
