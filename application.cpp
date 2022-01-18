#define TAOJSON
#include "application.h"

#include <utility>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
using namespace boost;
#include "cursor.h"

namespace drumlin {

Application *iapp;

/**
 * @brief Application::addThread : optionally start a thread as it is added
 * @param thread Thread*
 * @param start bool
 */
void Application::addThread(Thread *thread, bool startWork)
{
    THREADSLOCK
    {LOGLOCK;Debug() << __func__ << thread << ":" << *thread << (startWork ? "started" : "paused");}
    m_threads.push_back(thread);
    if (startWork)
    {
        thread->post(event::make_event(DrumlinEventThreadNotify,"beforeStart"));
        thread->start();
    }
}

/**
 * @brief Application::removeThread : remove a thread
 * @param _thread Thread*
 */
void Application::removeThread(Thread *thread)
{
    THREADSLOCK
    {LOGLOCK;Debug() << __func__ << thread->getTask();}
    if (thread->isStarted() && !thread->isTerminated()) {
        std::stringstream ss;
        ss << *thread << " : not terminated" << std::endl;
        throw Exception(ss);
    }
    auto endIter(std::remove(m_threads.begin(), m_threads.end(), thread));
    delete thread;
    {LOGLOCK;Debug() << "Number of remaining threads: " << std::distance(m_threads.begin(),endIter);}
    if(0==std::distance(m_threads.begin(),endIter)){
        post(event::make_event(DrumlinEventApplicationThreadsGone,"threadsGone",(Object*)0));
    }
}

void Application::post(std::shared_ptr<Event> pevent)
{
    CRITICAL;
    m_queue << pevent;
}

void Application::queuePromise(std::string name, Work::workPromise &&promise)
{
    PROMISELOCK;
    auto entry = make_pair(name, std::move(promise));
    m_promises.insert(std::move(entry));
}

int Application::exec()
{
    std::shared_ptr<Event> pevent;
    try {
        while(!terminated && !boost::this_thread::interruption_requested()){
            boost::this_thread::interruption_point();
            {
                boost::this_thread::disable_interruption di;
                if (m_queue.try_pull(pevent) != boost::queue_op_status::empty)
                {
                    if (!event(pevent))
                    {
                        Critical() << __func__ << "unhandled event" << *pevent;
                    }
                }
            }
            boost::this_thread::yield();
            boost::this_thread::sleep(boost::posix_time::milliseconds(1));
        }
    } catch(thread_interrupted &ti) {
        {LOGLOCK;Debug() << "thread interrupted: returning from Application::exec";}
        shutdown();
        return 1;
    } catch(Exception &e) {
        {LOGLOCK;Debug() << "Exception" << e << "returning from Application::exec";}
        return 2;
    } catch(std::exception &e) {
        {LOGLOCK;Debug() << "std::exception" << e << "returning from Application::exec";}
        shutdown();
        return 3;
    }
    {LOGLOCK;Debug() << "returning from Application::exec";}
    return 0;
}
/**
 * @brief Application::eventFilter : deal with events
 * @param obj Object*
 * @param event Event*
 * @return bool
 */
bool Application::event(std::shared_ptr<Event> const& pevent)
{
    EVENTLOG(pevent);

    try{
        if((guint32)pevent->type() < (guint32)DrumlinEventEvent_first
        || (guint32)pevent->type() > (guint32)DrumlinEventEvent_last){
            return false;
        }
        switch(pevent->type()){
        case DrumlinEventThreadWarning:
        {
            {LOGLOCK;Debug() << "Warning:" << pevent->getName();}
            break;
        }
        case DrumlinEventThreadRemove:
        {
            removeThread(pevent->getPointerVal<Thread>());
            break;
        }
        case DrumlinEventApplicationClose:
        {
            stop();
            //wait for threads
            break;
        }
        case DrumlinEventApplicationThreadsGone:
        {
            shutdown(pevent->getVal<bool>());
            //sends shutdown events
            break;
        }
        case DrumlinEventApplicationShutdown:
        {
            Tracer::endTrace();
            {LOGLOCK;Debug() << "Terminated...";}
            quit();
            //closes loop
            break;
        }
        case DrumlinEventApplicationRestart:
        {
            {LOGLOCK;Debug() << "Restarted...";}
            //create new app thread.
            break;
        }
        default:
            return false;
        }
        return true;
    }catch(Exception &e){
        {LOGLOCK;Debug() << e.what();}
    }
    return false;
}
/**
 * @brief Application::stop : stop the server
 */
void Application::stop()
{
    {LOGLOCK;Debug() << __FILE__ << __func__ << "Terminating threads...";}
    threads_type _threads;
    for(guint16 type = (guint16)ThreadType_last;type>(guint16)ThreadType_first;type--){
        _threads.clear();
        {
            THREADSLOCK
            std::copy_if(m_threads.begin(), m_threads.end(), std::back_inserter(_threads), [type](auto *thread){
                return thread->hasWorker() && thread->getWorker()->getType() == type;
            });
            {LOGLOCK;Debug()
                    << "Terminating "
                    << std::distance(_threads.begin(), _threads.end())
                    << " "
                    << metaEnum<ThreadType>().toString((gremlin::ThreadType)type)
                    << " threads";}
            for(auto thread : _threads){
                thread->terminate();
            }
        }
        {LOGLOCK;Debug()
                    << "Waiting for "
                    << std::distance(_threads.begin(), _threads.end())
                    << " "
                    << metaEnum<ThreadType>().toString((gremlin::ThreadType)type)
                    << " threads";}
        for(auto thread : _threads){
            if (!thread->isStarted())
            {
                thread->quit();
            } else {
                thread->wait();
            }
        }
    }
    {LOGLOCK;Debug() << __FILE__ << __func__ << "End of Application::stop.";}
}

void Application::quit()
{
    terminated = true;
}

void Application::shutdown(bool restarting)
{
    {LOGLOCK;Debug() << "Terminating...";}
    if(!restarting){
        post(event::make_event(DrumlinEventApplicationShutdown,"Shutdown::shutdown",(Object*)0));
    }else{
        post(event::make_event(DrumlinEventApplicationRestart,"Shutdown::restart",(Object*)0));
    }
}

bool Application::handleSignal(gremlin::SignalType signal)
{
    if(Tracer::tracer!=nullptr){
        Tracer::endTrace();
    }
    {LOGLOCK;Debug() << "punt DrumlinEventApplicationShutdown";}
    event::punt(event::make_event(DrumlinEventApplicationShutdown,
        gremlin::metaEnum<SignalType>().toString(signal),(Object*)(gint64)signal));
    return true;
}

void Application::getStatus(json::value *status)const
{
    THREADSLOCK;
    json::value array(json::empty_array);
    for(threads_type::value_type const& thread : m_threads){
        json::value obj(json::empty_object);
        thread->getWorker()->writeToObject(&obj);//report the thread
        array.get_array().push_back(obj);

        thread->getWorker()->getStatus(status);//report sub-system
    }
    status->get_object().insert({std::string("threads"),array});
}

} // namespace drumlin
