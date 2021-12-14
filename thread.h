#ifndef THREAD_H
#define THREAD_H

#include <chrono>
#include <mutex>
#include <memory>
#include <iostream>
#include <string>
#include <queue>
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
    typedef std::queue<std::shared_ptr<Event>> queue_type;
public:
    bool isTerminated(){ return m_terminated; }
    void terminate();
    /**
     * @brief  getBoostThread
     * @return const& boost::thread
     */
    boost::thread const& getBoostThread()const{ CRITICAL return *m_thread; }
    /**
     * @brief getWorker
     * @return ThreadWorker*
     */
    std::shared_ptr<ThreadWorker> getWorker()const{ CRITICAL return m_worker; }
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
    Thread(string _task, ThreadWorker *_worker);
    virtual ~Thread();
    double elapsed();
protected:
    virtual void start();
    virtual void run();
    virtual void exec();
    virtual bool event(std::shared_ptr<Event>);
public:
    virtual void quit();
    virtual void post(typename queue_type::value_type event);
    operator const char*()const;
    friend logger &operator<<(logger &stream,const Thread &rel);
    friend class ThreadWorker;
    void wait(gint64 millis = -1){
        if (!m_thread) {
          return;
        }
        if(!m_thread->joinable())
            return;
        if(millis<0)
            m_thread->join();
        else
            m_thread->try_join_for(boost::chrono::milliseconds(millis));
    }
private:
    queue_type m_queue;
    string m_task;
    std::shared_ptr<ThreadWorker> m_worker;
    bool m_ready = false;
    bool m_deleting = false;
    bool m_terminated = false;
    std::unique_ptr<boost::thread> m_thread;
    static std::recursive_mutex m_critical_section;
};

} // namespace drumlin

#endif // THREAD_H
