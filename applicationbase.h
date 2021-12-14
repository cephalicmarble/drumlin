#ifndef IAPPLICATION_H
#define IAPPLICATION_H

#include "tao_forward.h"
using namespace tao;
#include <mutex>
#include <vector>
using namespace std;
#include <boost/thread.hpp>
using namespace boost;
#include "thread.h"
#include "status.h"

namespace drumlin {

class Event;

typedef std::vector<Thread*> threads_type;

class ThreadAccessor;

class ApplicationBase : public StatusProvider
{
public:
    ApplicationBase(){}
    virtual ~ApplicationBase(){}
    virtual void post(std::shared_ptr<Event> event)=0;
    virtual void stop()=0;
    boost::thread::id getThreadId(){ return boost::this_thread::get_id(); }
    /**
     * @brief Server::getStatus : return a list.join("\n") of running threads
     * @return const char*
     */
    virtual void getStatus(json::value *status)const;
    friend class ThreadAccessor;
protected:
    std::mutex m_critical_section;
    threads_type threads;
};

extern ApplicationBase *iapp;

}

#endif // IAPPLICATION_H
