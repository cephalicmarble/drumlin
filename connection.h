#ifndef CONNECTION_H
#define CONNECTION_H

#include <memory>
using namespace std;
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
using namespace boost;
#include "glib.h"
#include "socket.h"
#include "exception.h"

namespace drumlin {

extern boost::asio::io_service io_service;

class IOService
{
public:
    IOService():m_thread(&IOService::run,this){}
    void run(){io_service.run();}
    void stop(){io_service.stop();}
private:
    boost::thread m_thread;
};

extern unique_ptr<IOService> io_thread;
extern void start_io();

template <class Protocol = asio::ip::tcp>
class AsioClient
{
public:
    typedef Protocol protocol_type;
    typedef typename protocol_type::resolver resolver_type;
    typedef typename Protocol::endpoint Endpoint;
    typedef typename Protocol::socket Socket;
    AsioClient(typename resolver_type::iterator resolver_iter)
        :m_endpoint(*resolver_iter)
    {
    }
    AsioClient(string host,int port)
        :m_endpoint(asio::ip::address::from_string(host),port)
    {
    }
    Socket getAsioSocket()
    {
        Socket socket(drumlin::io_service);
        socket.connect(m_endpoint);
        return socket;
    }
protected:
    Endpoint m_endpoint;
//    static AsioClient<Protocol> resolve(boost::asio::io_service,string host,int port)
//    {
//        resolver_type resolver(io_service);
//        typename protocol_type::resolver::query query(host,port);
//        typename resolver_type::iterator iter = resolver.resolve(query);
//        typename resolver_type::iterator end;
//        while(iter != end){
//            typename protocol_type::endpoint endpoint = *iter++;
//            std::cout << endpoint << std::endl;
//        }
//        return true;
//    }
};

template <class Protocol = asio::ip::tcp>
class Connection
  : public boost::enable_shared_from_this<Connection<Protocol>>
{
public:
    typedef typename Protocol::socket asio_socket_type;

    const asio_socket_type *p_asio_socket()const
    {
        return &m_asio_socket;
    }
    asio_socket_type & asio_socket()
    {
        return m_asio_socket;
    }

    Connection()
        : m_asio_socket(io_service)
    {
    }
    Connection(asio_socket_type &&socket)
        : m_asio_socket(std::move(socket))
    {
    }
    ~Connection()
    {
    }
private:
    asio_socket_type m_asio_socket;
};

template <
        class Server,
        class Handler,
        class Address = asio::ip::address_v4,
        class Protocol = asio::ip::tcp
        >
class AsioServer
{
public:
    typedef Address address_type;
    typedef Connection<Protocol> connection_type;
    typedef typename Protocol::endpoint endpoint_type;
    typedef typename Protocol::socket socket_type;
    typedef typename Protocol::acceptor acceptor_type;
    typedef typename Protocol::resolver resolver_type;
    typedef AsioServer<Server,Handler,address_type,Protocol> server_type;
    AsioServer(string host,int port):m_addr(),m_endpoint(host.length()?m_addr.from_string(host):m_addr.any(),port),m_acceptor(acceptor_type(drumlin::io_service,m_endpoint))
    {
    }
    void start()
    {
        m_acceptor.listen(10);
        connection_type *new_connection = new connection_type();
//        m_acceptor.accept(new_connection->socket().socket());
//        cout << new_connection->socket().socket().available() << endl;
//        new_connection->socket().socket().send(asio::buffer(string("BLARGLE")));
        m_acceptor.async_accept(new_connection->asio_socket(),
            boost::bind(&server_type::handle_accept, this, new_connection, boost::asio::placeholders::error));
    }
    virtual void handle_accept(connection_type *new_connection,boost::system::error_code error)
    {
        if(!error)
        {
            new Handler((Server*)this,new_connection);
            start();
        }else{
            Critical() << error.message();
            delete new_connection;
        }
    }
    address_type m_addr;
    endpoint_type m_endpoint;
    acceptor_type m_acceptor;
    void stop()
    {
        m_acceptor.close();
    }
};

} // namespace drumlin

#endif // CONNECTION_H
