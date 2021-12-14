#include "connection.h"
namespace drumlin {
    boost::asio::io_service io_service;

    IOService::IOService():m_thread(&IOService::run,this)
    {
        CPLATE;
    }
    IOService::~IOService()
    {
        DPLATE;
    }

    void IOService::run()
    {
        io_service.start_io();
    }

    void IOService::stop()
    {
        io_service.stop();
    }

    unique_ptr<IOService> io_thread;
    void start_io(){
        if(!!io_thread)
            return;
        io_thread.reset(new IOService());
    }
    void stop_io(){
        io_thread->stop();
        io_thread.reset(nullptr);
    }
}
