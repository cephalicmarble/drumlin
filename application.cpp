#include "application.h"

#include <boost/thread.hpp>
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
    Debug() << __func__ << thread->getTask() << *thread;
    threads.add(thread->getTask(),thread->getWorker());
}
/**
 * @brief Application::findThread : find threads of name or "all" of type
 * @param name string maybe "all"
 * @param type ThreadWorker::ThreadType
 * @return std::vector<Thread*>
 */
threads_type Application::findThread(const string &name,ThreadWorker::Type type)
{
    THREADSLOCK
    threads_type _threads;
    for(auto thread : threads){
        if(thread.first == name || (name == "all" && thread.second->getType() == type))
            _threads.push_back(thread.second->getThread());
    }
    return _threads;
}
/**
 * @brief getThreads
 * @param type ThreadWorker::Type
 * @return
 */
threads_type Application::getThreads(ThreadWorker::Type type)
{
    THREADSLOCK
    threads_type found;
    for(auto thread : threads){
        if(thread.second->getType() == type)
            found.push_back(thread.second->getThread());
    }
    return found;
}

/**
 * @brief Application::removeThread : remove a thread
 * @param _thread Thread*
 */
void Application::removeThread(Thread *thread,bool noDelete)
{
    THREADSLOCK
    Debug() << __func__ << thread->getTask();
    threads.remove(thread->getTask(),true);
    if(!noDelete){
        thread->wait();
        delete thread;
    }
    if(0==std::distance(threads.begin(),threads.end())){
        make_event(DrumlinEventApplicationThreadsGone,"threadsGone",(Object*)0)->punt();
    }
}

void Application::post(Event *pevent)
{
    if(terminated){
        if(event(pevent))
            delete pevent;
    }else{
        m_queue << pevent;
    }
}

int Application::exec()
{
    Event *pevent;
    try{
        while(!terminated && !boost::this_thread::interruption_requested()){
            boost::this_thread::interruption_point();
            {
                boost::this_thread::disable_interruption di;
                m_queue.wait_pull(pevent);
                if(event(pevent)){
                    delete pevent;
                }else{
                    Critical() << __func__ << "leaking event" << *pevent;
                }
            }
        }
    }catch(thread_interrupted &ti){
        shutdown();
        return 1;
    }catch(...){
        shutdown();
        return 2;
    }
    return 0;
}
/**
 * @brief Application::eventFilter : deal with events
 * @param obj Object*
 * @param event Event*
 * @return bool
 */
bool Application::event(Event *pevent)
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
            removeThread(event_cast<Thread>(pevent)->getPointer());
            break;
        }
        case DrumlinEventApplicationClose:
        {
            shutdown((bool)event_cast<Object>(pevent)->getPointer());
            break;
        }
        case DrumlinEventApplicationThreadsGone:
        {
            Tracer::endTrace();
            if(terminator){
                post(new Event(DrumlinEventApplicationShutdown));
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
    for(guint16 type = (guint16)ThreadType_terminator-1;type>(guint16)ThreadType_first;type--){
        threads_type threads(getThreads((ThreadWorker::Type)type));
        for(threads_type::value_type &thread : threads){
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

bool Application::handleSignal(int signal)
{
    if(Tracer::tracer!=nullptr){
        Tracer::endTrace();
    }
    make_event(DrumlinEventApplicationShutdown,"Signal::shutdown",(Object*)(gint64)signal)->punt();
    return true;
}

} // namespace drumlin
