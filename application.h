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

namespace drumlin {

extern ApplicationBase *iapp;

#define THREADSLOCK std::lock_guard<std::mutex> l(const_cast<std::mutex&>(m_critical_section));

class Application :
        public SignalHandler,
        public ApplicationBase
{
public:
    Application() {;}
    virtual ~Application() {;}

    /**
     * @brief Application::addThread : optionally start a thread as it is added
     * @param thread Thread*
     * @param start bool
     */
    void addThread(Thread *thread);
    /**
     * @brief Application::findThread : find threads of name or "all" of type
     * @param name string maybe "all"
     * @param type ThreadWorker::ThreadType
     * @return std::vector<Thread*>
     */
    threads_type findThread(const string &name,ThreadWorker::Type type);
    /**
     * @brief getThreads
     * @param type ThreadWorker::Type
     * @return
     */
    threads_type getThreads(ThreadWorker::Type type);

    /**
     * @brief Application::removeThread : remove a thread
     * @param _thread Thread*
     */
    void removeThread(Thread *thread,bool noDelete = false);

    void post(Event *pevent);

    int exec();
    /**
     * @brief Application::eventFilter : deal with events
     * @param obj Object*
     * @param event Event*
     * @return bool
     */
    virtual bool event(Event *pevent);
    /**
     * @brief Application::stop : stop the server
     */
    virtual void stop();
    void quit();
    void shutdown(bool restarting = false);
    bool handleSignal(int signal);
protected:
    Terminator *terminator = nullptr;
private:
    bool terminated = false;
    boost::concurrent::sync_queue<Event*> m_queue;
};

} // namespace drumlin

#endif // APPLICATION_H
