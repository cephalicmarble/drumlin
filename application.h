#ifndef APPLICATION_H
#define APPLICATION_H

#include "tao_forward.h"
using namespace tao;
#include <mutex>
using namespace std;
#include <boost/thread/sync_queue.hpp>
#include "drumlin.h"
#include "event.h"
#include "signalhandler.h"
#include "thread.h"
#include "thread_worker.h"
#include "metatypes.h"
#include "work.h"

namespace drumlin {

class ThreadsAccessor;

typedef std::vector<Thread*> threads_type;

class Application :
        public SignalHandler,
        public Work::Promises
{
public:
    Application() : SignalHandler(gremlin::SignalType_all) {
        APLATE;
    }
    virtual ~Application() {
        BPLATE;
    }

    /**
     * @brief Application::addThread : optionally start a thread as it is added
     * @param thread Thread*
     * @param start bool
     */
    virtual void addThread(Thread *thread, bool startWork);

    /**
     * @brief Application::removeThread : remove a thread
     * @param _thread Thread*
     */
    virtual void removeThread(Thread *thread);

    virtual void post(std::shared_ptr<Event> event);

    virtual void queuePromise(std::string name, Work::workPromise &&promise);

    int exec();
    /**
     * @brief Application::eventFilter : deal with events
     * @param obj Object*
     * @param event Event*
     * @return bool
     */
    virtual bool event(std::shared_ptr<Event> const& pevent);
    /**
     * @brief Application::stop : stop the server
     */
    virtual void stop();
    void quit();
    void shutdown(bool restarting = false);
    bool handleSignal(gremlin::SignalType signal);

    boost::thread::id getThreadId(){ return boost::this_thread::get_id(); }
    /**
     * @brief Server::getStatus : return a list.join("\n") of running threads
     * @return const char*
     */
    virtual void getStatus(json::value *status)const;
    friend class ThreadAccessor;

private:
    std::recursive_mutex m_critical_section;
    threads_type m_threads;
    bool terminated = false;
    boost::concurrent::sync_queue<std::shared_ptr<Event>> m_queue;
    Work::promise_map_type m_promises;

    friend class ThreadsAccessor;
};

extern Application *iapp;

} // namespace drumlin

#endif // APPLICATION_H
