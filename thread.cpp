#define TAOJSON
#include "thread.h"

#include <sstream>
#include <sys/time.h>
#ifdef LINUX
#include <sys/resource.h>
#endif
#include <mutex>
using namespace std;
#include <boost/thread/sync_queue.hpp>
using namespace boost;
#include "drumlin.h"
#include "event.h"
#include "application.h"
#include "metatypes.h"

namespace drumlin {

/**
 * @brief Thread::Thread : construct a new thread, to be Application::addThread(*,bool start)-ed
 * @param _task string name
 */
Thread::Thread(string _task, ThreadWorker *_worker) : m_task(_task), m_worker(_worker)
{
}

/**
 * @brief Thread::~Thread
 */
Thread::~Thread()
{
    Debug() << __func__ << this;
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
bool Thread::event(std::shared_ptr<Event> pevent)
{
    if(!pevent.get())
        return false;
    if(getWorker()->event(pevent)){
        return true;
    }
    quietDebug() << this << __func__ << metaEnum<DrumlinEventType>().toString((DrumlinEventType)pevent->type());
    if((guint32)pevent->type() < (guint32)DrumlinEventEvent_first
            || (guint32)pevent->type() > (guint32)DrumlinEventEvent_last){
        return false;
    }
    if((guint32)pevent->type() > (guint32)DrumlinEventThread_first
            && (guint32)pevent->type() < (guint32)DrumlinEventThread_last){
        quietDebug() << pevent->getName();
        switch(pevent->type()){
        case DrumlinEventThreadWork:
        {
            getWorker()->work(pevent->getPointerVal<Object>(),pevent);
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

void Thread::start()
{
    m_ready = true;
    m_thread.reset(new boost::thread(&Thread::run,this));
}

/**
 * @brief Thread::run : thread function
 * Sits and processes events, yielding to interruption and delayed stoppage from stop().
 * Installs the thread as its own eventFilter (qv)
 * Signals getWorker()->run(Object*,Event*) to do work.
 */
void Thread::run()
{
    post(event::make_event(DrumlinEventThreadWork,"work"));
    if(!m_terminated)
        exec();
    if(getWorker()){
        m_worker->shutdown();
        m_worker.reset();
    }else{
        terminate();
    }
    m_ready = false;
    event::punt(event::make_event(DrumlinEventThreadRemove,"removeThread",this));
}

void Thread::post(typename queue_type::value_type event)
{
    std::lock_guard<std::recursive_mutex> l(m_critical_section);
    m_queue.push(event);
}

void Thread::exec()
{
    while(!m_deleting && !m_terminated && !m_thread->interruption_requested()){
        try{
            boost::this_thread::interruption_point();
            queue_type::value_type pevent;
            {
                std::lock_guard<std::recursive_mutex> l(m_critical_section);
                if(!m_queue.empty()) {
                    pevent = m_queue.front();
                    m_queue.pop();
                }
            }
            if(!!pevent) {
                if(!event(pevent))
                    Critical() << __func__ << "not handling event" << *pevent;
            }
            boost::this_thread::yield();
        }catch(thread_interrupted &ti){
        }
    }
}

void Thread::quit()
{
    m_thread->interrupt();
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

std::recursive_mutex Thread::m_critical_section;

} // namespace drumlin
