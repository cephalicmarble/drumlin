#include <drumlin.h>
#include <tao/json.hpp>
using namespace tao;
#include "thread.h"
#include "event.h"
#include "application.h"
#include "metatypes.h"
#include <sstream>
#include <sys/time.h>
#ifdef LINUX
#include <sys/resource.h>
#endif
#include <mutex>
using namespace std;
#include <boost/thread/sync_queue.hpp>
using namespace boost;

namespace drumlin {

/**
 * @brief Thread::Thread : construct a new thread, to be Application<T>::addThread(*,bool start)-ed
 * @param _task string name
 */
Thread::Thread(string _task) : m_thread(&Thread::run,this),m_task(_task)
{
}

/**
 * @brief Thread::~Thread
 */
Thread::~Thread()
{
    Debug() << __func__ << this;
    m_deleting = true;
    if(!m_ready)
        return;
    if(getWorker())
        delete getWorker();
}

void Thread::terminate()
{
    Debug() << __func__ << this;
    {
        if(m_terminated)
            return;
        m_terminated = true;
    }
    quit();
}

/**
 * @brief Thread::elapsed : get thread-specific elapsed time
 * @return quint64
 */
double Thread::elapsed()
{
#ifdef LINUX
    static timespec spec;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID,&spec);
    return spec.tv_nsec / 1000000.0;
#endif
    return 0.0;
}

/**
 * @brief Thread::getName : (string)"<type>:<task>"
 * @return string
 */
string Thread::getName()
{
    return metaEnum<ThreadWorker::Type>().toString(m_worker->getType()) + ":" + m_task;
}

/**
 * @brief Thread::event : deal with thread messages
 * @param obj Object*
 * @param event Event*
 * @return bool
 */
bool Thread::event(Event *pevent)
{
    if(!pevent)
        return false;
    if(getWorker()->event(pevent)){
        return true;
    }
    quietDebug() << this << __func__ << metaEnum<DrumlinEventType>().toString((DrumlinEventType)pevent->type());
    if((unsigned int)pevent->type() < (unsigned int)DrumlinEventEvent_first
            || (unsigned int)pevent->type() > (unsigned int)DrumlinEventEvent_last){
        return false;
    }
    if((unsigned int)pevent->type() > (unsigned int)DrumlinEventThread_first
            && (unsigned int)pevent->type() < (unsigned int)DrumlinEventThread_last){
        quietDebug() << pevent->getName();
        switch(pevent->type()){
        case DrumlinEventThreadWork:
        {
            getWorker()->work(event_cast<Object>(pevent)->getPointer(),pevent);
            break;
        }
        case DrumlinEventThreadShutdown:
        {
            quit();
            break;
        }
        default:
            quietDebug() << metaEnum<DrumlinEventType>().toString((DrumlinEventType)pevent->type()) << __FILE__ << __func__ <<  "unimplemented";
            return false;
        }
        return true;
    }
    return false;
}

/**
 * @brief Thread::run : thread function
 * Sits and processes events, yielding to interruption and delayed stoppage from stop().
 * Installs the thread as its own eventFilter (qv)
 * Signals getWorker()->run(Object*,Event*) to do work.
 */
void Thread::run()
{
    while(!getWorker() || !getWorker()->m_critical_section.try_lock()){
        boost::this_thread::sleep_for(boost::chrono::milliseconds(400));
    }
    getWorker()->m_critical_section.unlock();
    {
        m_ready = true;
    }
    make_event(DrumlinEventThreadWork,"work")->send(this);
    if(!m_terminated)
        exec();
    terminate();
    if(getWorker()){
        m_worker->shutdown();
        delete m_worker;
        m_worker = nullptr;
    }
    make_event(DrumlinEventThreadRemove,"removeThread",this)->punt();
}

void Thread::post(Event *event)
{
    std::lock_guard<std::recursive_mutex> l(m_critical_section);
    m_queue.push(event);
}

void Thread::exec()
{
    while(!m_deleting && !m_terminated && !m_thread.interruption_requested()){
        try{
            boost::this_thread::interruption_point();
            queue_type::value_type pevent(nullptr);
            {
                std::lock_guard<std::recursive_mutex> l(m_critical_section);
                if(!m_queue.empty()) {
                    pevent = m_queue.front();
                    m_queue.pop();
                }
            }
            if(pevent != nullptr) {
                if(event(pevent)){
                    delete pevent;
                }else{
                    Critical() << __func__ << "leaking event" << *pevent;
                }
            }
            boost::this_thread::yield();
        }catch(thread_interrupted &ti){
        }
    }
}

void Thread::quit()
{
    m_thread.interrupt();
}

Thread *Thread::setTask(string _task)
{
    iapp->renameThread(this,_task);
    m_task = _task;
    return this;
}

void ThreadWorker::stop()
{
    shutdown();
    signalTermination();
}

ThreadWorker::ThreadWorker(Type _type,Object *parent) : Object(parent),m_type(_type)
{
}

ThreadWorker::ThreadWorker(Type _type,string task) : Object(),m_type(_type)
{
    startThread(task);
}

/**
 * @brief ThreadWorker::ThreadWorker : worker constructor
 * connects thread->finished() to punt event back to application for removal.
 * Sets worker to this, and moves the worker to the (already created) thread.
 * @param _thread Thread*
 */
ThreadWorker::ThreadWorker(Type _type,Thread *_thread) : Object(),m_thread(_thread),m_type(_type)
{
    std::lock_guard<std::recursive_mutex> l(m_critical_section);
    m_thread = _thread;
    m_thread->setWorker(this);
}

/**
 * @brief ThreadWorker::~ThreadWorker : removes the event filter
 */
ThreadWorker::~ThreadWorker()
{
    if(m_thread)
        m_thread->m_worker = nullptr;
}

void ThreadWorker::startThread(string task)
{
    std::lock_guard<std::recursive_mutex> l(m_critical_section);
    m_thread = new Thread(task);
    m_thread->setWorker(this);
}

/**
 * @brief ThreadWorker::signalTermination
 */
void ThreadWorker::signalTermination()
{
    if(!getThread()->isTerminated())
        getThread()->terminate();
}

void ThreadWorker::getStatus(json::value *obj)const
{
    auto &map(obj->get_object());
    map.insert({"task",getThread()->getTask()});
    map.insert({"type",string(metaEnum<Type>().toString(this->m_type))});
    map.insert({"elapsed",getThread()->elapsed()});
    map.insert({"memory",0});
    if(map.end()==map.find("jobs"))
        map.insert({"jobs",json::empty_object});
    for(jobs_type::value_type const& job : m_jobs){
        json::value job_obj(json::empty_object);
        job.second->getStatus(&job_obj);
        map.at("jobs").get_object().insert({job.first,job_obj});
    }
}

void ThreadWorker::postWork(Object *sender)
{
    make_event(DrumlinEventThreadWork,__func__,sender)->send(getThread());
}

/* STREAM OPERATORS */

void ThreadWorker::writeToObject(json::value *obj)const
{
    getStatus(obj);
    obj->get_object().insert({"task",getThread()->getTask()});
}

void ThreadWorker::writeToStream(std::ostream &stream)const
{
    json::value obj(json::empty_object);
    writeToObject(&obj);
    json::to_stream(stream,obj);
}

/**
 * @brief operator const char*: for Debug
 */
Thread::operator const char*()const
{
    std::lock_guard<std::recursive_mutex> l(const_cast<std::recursive_mutex&>(m_critical_section));
    static char *buf;
    if(buf)free(buf);
    std::stringstream ss;
    ss << *this;
    return buf = strdup(ss.str().c_str());
}

/**
 * @brief operator << : stream operator
 * @param stream std::ostream &
 * @param rhs Thread const&
 * @return std::ostream &
 */
logger &operator<<(logger &stream,const Thread &rel)
{
    stream << rel.getBoostThread().get_id();
    auto tmp = rel.getWorker();
    if(tmp){
        tmp->writeToStream((ostream&)stream);
    }
    return stream;
}

} // namespace drumlin
