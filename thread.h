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

/**
 * @brief The Thread class
 */
class Thread
{
    typedef std::queue<std::shared_ptr<Event>> queue_type;
public:
    bool isTerminated()const{ return m_terminated; }
    void terminate();
    boost::thread const& getBoostThread()const;
    std::shared_ptr<ThreadWorker> getWorker()const;
    bool hasWorker()const;
    bool isStarted()const;
    string getType()const;
    /**
     * @brief getTask
     * @return string
     */
    string getTask()const{ CRITICAL return m_task; }
    Thread(string _task, ThreadWorker *_worker);
    virtual ~Thread();
    double elapsed();
    virtual void start();
protected:
    virtual void run();
    virtual void exec();
    virtual bool event(std::shared_ptr<Event>);
public:
    virtual void quit();
    virtual void post(typename queue_type::value_type event);
    operator const char*()const;
    friend class ThreadWorker;
    void wait(gint64 millis = -1);
private:
    queue_type m_queue;
    string m_task;
    std::shared_ptr<ThreadWorker> m_worker;
    bool m_ready = false;
    bool m_deleting = false;
    bool m_terminated = false;
    std::unique_ptr<boost::thread> m_thread;
    static std::recursive_mutex m_critical_section;

    friend std::ostream &operator <<(std::ostream &stream, const Thread &e);
};

extern std::ostream &operator <<(std::ostream &stream, const Thread &e);

} // namespace drumlin

#endif // THREAD_H
