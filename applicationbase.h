#ifndef IAPPLICATION_H
#define IAPPLICATION_H

#include <tao_forward.h>
using namespace tao;
#include <mutex>
using namespace std;
#include <boost/thread.hpp>
using namespace boost;
#include "thread.h"

namespace drumlin {

class Event;

typedef std::vector<Thread*> threads_type;

class ApplicationBase : public StatusProvider
{
public:
    ApplicationBase(){}
    virtual ~ApplicationBase(){}
    virtual void post(Event *event)=0;
    virtual void stop()=0;
    boost::thread::id getThreadId(){ return boost::this_thread::get_id(); }
    virtual void addThread(Thread *thread)=0;
    virtual threads_type findThread(const string &name,ThreadWorker::Type type)=0;
    virtual void renameThread(Thread *thread,string const& _new)=0;
    virtual void removeThread(Thread *thread,bool noDelete = false)=0;
    /**
     * @brief Server::getStatus : return a list.join("\n") of running threads
     * @return const char*
     */
    virtual void getStatus(json::value *status)const;
    typedef Registry<ThreadWorker> threads_reg_type;
    std::mutex m_critical_section;
protected:
    threads_reg_type threads;
};

extern ApplicationBase *iapp;

}

#endif // IAPPLICATION_H
