#include "application.h"

#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
using namespace boost;
#include "cursor.h"

namespace drumlin {

ApplicationBase *iapp;

/**
 * @brief Application::addThread : optionally start a thread as it is added
 * @param thread Thread*
 * @param start bool
 */
void Application::addThread(Thread *thread)
{
    THREADSLOCK
    Debug() << __func__ << thread->getTask() << ":" << thread->getName();
    threads.push_back(thread);
}

/**
 * @brief Application::removeThread : remove a thread
 * @param _thread Thread*
 */
void Application::removeThread(Thread *thread)
{
    THREADSLOCK
    Debug() << __func__ << thread->getTask();
    if (thread->isStarted() && !thread->isTerminated()) {
        std::stringstream ss;
        ss << "thread (" << thread->getName() << ") not terminated." << std::endl;
        throw Exception(ss);
    }
    (void)std::remove(threads.begin(), threads.end(), thread);
    delete thread;
    if(0==std::distance(threads.begin(),threads.end())){
        post(event::make_event(DrumlinEventApplicationThreadsGone,"threadsGone",(Object*)0));
    }
}

void Application::post(std::shared_ptr<Event> pevent)
{
    if(terminated) {
        event(pevent);
    } else {
        m_queue << pevent;
    }
}

int Application::exec()
{
    std::shared_ptr<Event> pevent;
    try{
        while(!terminated && !boost::this_thread::interruption_requested()){
            boost::this_thread::interruption_point();
            {
                boost::this_thread::disable_interruption di;
                m_queue.wait_pull(pevent);
                if(!event(pevent)){
                    Critical() << __func__ << "unhandled event" << *pevent;
                }
            }
        }
    }catch(thread_interrupted &ti){
        Debug() << "thread interrupted: returning from Application::exec";
        shutdown();
        return 1;
    }catch(...){
        Debug() << "caught exception: returning from Application::exec";
        shutdown();
        return 2;
    }
    Debug() << "returning from Application::exec";
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
    try{
        if((guint32)pevent->type() < (guint32)DrumlinEventEvent_first
        || (guint32)pevent->type() > (guint32)DrumlinEventEvent_last){
            return false;
        }
        switch(pevent->type()){
        case DrumlinEventThreadWarning:
        {
            Debug() << pevent->getName();
            break;
        }
        case DrumlinEventThreadRemove:
        {
            removeThread(pevent->getPointerVal<Thread>());
            break;
        }
        case DrumlinEventApplicationClose:
        {
            shutdown(pevent->getVal<bool>());
            break;
        }
        case DrumlinEventApplicationThreadsGone:
        {
            Tracer::endTrace();
            if(terminator){
                post(event::make_event(DrumlinEventApplicationShutdown, "threads-gone"));
            }
            break;
        }
        case DrumlinEventApplicationShutdown:
        {
            Debug() << "Terminated...";
            quit();
            break;
        }
        case DrumlinEventApplicationRestart:
        {
            Debug() << "Restarted...";
            terminated = false;
            exec();
            break;
        }
        default:
            return false;
        }
        return true;
    }catch(Exception &e){
        Debug() << e.what();
    }
    return false;
}
/**
 * @brief Application::stop : stop the server
 */
void Application::stop()
{
    Debug() << this << __func__;
    threads_type _threads;
    for(guint16 type = (guint16)ThreadType_terminator-1;type>(guint16)ThreadType_first;type--){
        threads.clear();
        std::copy_if(m_threads.begin(), m_threads.end(), std::back_inserter(threads), [type](auto *thread){
            return thread->getWorker()->getType() == type;
        });
        for(threads_type::value_type &thread : _threads){
            thread->terminate();
            thread->wait(-1);
        }
    }
}

void Application::quit()
{
    terminated = true;
}

void Application::shutdown(bool restarting)
{
    Debug() << "Terminating...";
    terminator = new Terminator(restarting);
}

bool Application::handleSignal(gremlin::SignalType signal)
{
    if(Tracer::tracer!=nullptr){
        Tracer::endTrace();
    }
    Debug() << "punt DrumlinEventApplicationShutdown";
    event::punt(event::make_event(DrumlinEventApplicationShutdown,
        gremlin::metaEnum<SignalType>().toString(signal),(Object*)(gint64)signal));
    return true;
}

} // namespace drumlin
