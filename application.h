#ifndef APPLICATION_H
#define APPLICATION_H

#include "tao_forward.h"
using namespace tao;
#include <mutex>
using namespace std;
#include <boost/thread/sync_queue.hpp>
#include "applicationbase.h"
#include "event.h"
#include "signalhandler.h"
#include "thread.h"
#include "thread_worker.h"
#include "terminator.h"
#include "metatypes.h"

namespace drumlin {

extern ApplicationBase *iapp;

#define FRIENDTHREADSLOCK std::lock_guard<std::recursive_mutex> l(const_cast<std::recursive_mutex&>(iapp->m_critical_section));
#define THREADSLOCK std::lock_guard<std::recursive_mutex> l(const_cast<std::recursive_mutex&>(m_critical_section));

class ThreadsAccessor;

class Application :
        public SignalHandler,
        public ApplicationBase
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
    void addThread(Thread *thread, bool startWork);

    /**
     * @brief Application::removeThread : remove a thread
     * @param _thread Thread*
     */
    void removeThread(Thread *thread);

    virtual void post(std::shared_ptr<Event> event);

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
private:
    bool terminated = false;
    boost::concurrent::sync_queue<std::shared_ptr<Event>> m_queue;

    friend class ThreadsAccessor;
};

} // namespace drumlin

#endif // APPLICATION_H
