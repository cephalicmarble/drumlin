#ifndef THREAD_H
#define THREAD_H

#include <iostream>
#include <string>
#include <queue>
#include <mutex>
#include <chrono>
using namespace std;
#include <boost/thread.hpp>
#include <boost/thread/sync_queue.hpp>
using namespace boost;
#include "drumlin.h"
#include "object.h"
#include "registry.h"
#include "gtypes.h"
using namespace drumlin;
#include "../gremlin/compat.h"
#include "thread_worker.h"

namespace drumlin {

/*
 * Forward declarations
 */
class ThreadWorker;
class Event;
class Server;

#define CRITICAL std::lock_guard<std::recursive_mutex> l(const_cast<std::recursive_mutex&>(m_critical_section));

/**
 * @brief The Thread class
 */
class Thread
{
public:
    typedef queue<Event*> queue_type;
    bool isTerminated(){ return m_terminated; }
    void terminate();
    std::recursive_mutex m_critical_section;
    Thread(string _task);
    boost::thread const& getBoostThread()const{ CRITICAL return m_thread; }
    /**
     * @brief setWorker
     * @param _worker ThreadWorker*
     */
    void setWorker(ThreadWorker *_worker){ CRITICAL m_worker = _worker; }
    /**
     * @brief getWorker
     * @return ThreadWorker*
     */
    ThreadWorker *getWorker()const{ CRITICAL return m_worker; }
    /**
     * @brief isStarted
     * @return bool
     */
    bool isStarted()const{ return m_ready; }
    string getName();
    /**
     * @brief getTask
     * @return string
     */
    string getTask()const{ CRITICAL return m_task; }
    /**
     * @brief setTask
     * @param _task string
     * @return Thread*
     */
    Thread *setTask(string _task){ m_task = _task; return this; }
    virtual ~Thread();
    double elapsed();
protected:
    virtual void run();
    virtual void exec();
    virtual bool event(Event *);
public:
    virtual void quit();
    void post(Event *);
    operator const char*()const;
    friend logger &operator<<(logger &stream,const Thread &rel);
    friend class ThreadWorker;
    void wait(gint64 millis = -1){
        if(!m_thread.joinable())
            return;
        if(millis<0)
            m_thread.join();
        else
            m_thread.try_join_for(boost::chrono::milliseconds(millis));
    }
private:
    queue_type m_queue;
    ThreadWorker *m_worker = nullptr;
    bool m_ready = false;
    bool m_deleting = false;
    bool m_terminated = false;
    boost::thread m_thread;
    string m_task;
};

} // namespace drumlin

#endif // THREAD_H
