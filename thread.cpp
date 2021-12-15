#define TAOJSON
#include "thread.h"

#include <sstream>
#include <sys/time.h>
#ifdef LINUX
#include <sys/resource.h>
#endif
#include <mutex>
using namespace std;
#include <boost/preprocessor.hpp>
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
    APLATE;
}

/**
 * @brief Thread::~Thread
 */
Thread::~Thread()
{
    BPLATE;
}

void Thread::terminate()
{
    PLATE;
    {
        CRITICAL;
        if(m_terminated)
            return;
        m_terminated = true;
    }
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
 * @brief  getBoostThread
 * @return const& boost::thread
 */
boost::thread const& Thread::getBoostThread()const
{
    CRITICAL;
    return *m_thread;
}

/**
 * @brief getWorker
 * @return ThreadWorker*
 */
std::shared_ptr<ThreadWorker> Thread::getWorker()const
{
    CRITICAL
    return m_worker;
}

/**
 * @brief hasWorker
 * @return bool
 */
bool Thread::hasWorker()const
{
    CRITICAL
    return m_worker.get() != nullptr;
}

/**
 * @brief isStarted
 * @return bool
 */
bool Thread::isStarted()const
{
    return m_ready;
}

/**
 * @brief Thread::getName : (string)"<type>:<task>"
 * @return string
 */
string Thread::getType()const
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
    if((guint32)pevent->type() < (guint32)DrumlinEventEvent_first
            || (guint32)pevent->type() > (guint32)DrumlinEventEvent_last){
        return false;
    } else {
        THREADLOG2(metaEnum<DrumlinEventType>().toString((DrumlinEventType)pevent->type()),pevent->getName());
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
            THREADLOG2(metaEnum<DrumlinEventType>().toString((DrumlinEventType)pevent->type()),"unimplemented");
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
    post(event::make_event(DrumlinEventThreadNotify,"beforeWork"));
    THREADLOG2("starting",boost::this_thread::get_id())
    post(event::make_event(DrumlinEventThreadWork,"work"));
    if(!m_terminated)
        exec();
    if(getWorker()){
        {LOGLOCK;Debug() << *this << " resetting worker...";}
        m_worker->shutdown();
        m_worker.reset();
    }else{
        terminate();
    }
    m_ready = false;
    quit();
}

void Thread::post(typename queue_type::value_type event)
{
    std::lock_guard<std::recursive_mutex> l(m_critical_section);
    m_queue.push(event);
}

void Thread::exec()
{
    while(m_ready && !m_deleting && !m_terminated && !m_thread->interruption_requested()){
        queue_type::value_type pevent;
        try {
            boost::this_thread::interruption_point();
            {
                std::lock_guard<std::recursive_mutex> l(m_critical_section);
                if(!m_queue.empty()) {
                    pevent = m_queue.front();
                    m_queue.pop();
                }
            }
            if(!!pevent) {
                if(!event(pevent))
                    {LOGLOCK;Critical() << __func__ << "not handling event" << *pevent;}
            }
        } catch(thread_interrupted &ti) {
            THREADLOG2(*pevent, "thread_interrupted");
            break;
        } catch(Exception const& e) {
            THREADLOG2(*pevent, e);
            break;
        } catch(std::exception const& e) {
            THREADLOG2(*pevent, e);
            break;
        }
        boost::this_thread::yield();
        boost::this_thread::sleep(boost::posix_time::milliseconds(400));
    }
    m_deleting = true;
}

void Thread::quit()
{
    CRITICAL;
    //m_thread->interrupt();
    event::punt(event::make_event(DrumlinEventThreadRemove,"removeThread",this));
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

void Thread::wait(gint64 millis){
    if (!m_thread) {
      return;
    }
    if(!m_deleting && m_thread->joinable())
    {
        if(millis<0)
            m_thread->join();
        else
            m_thread->try_join_for(boost::chrono::milliseconds(millis));
    }
}

/**
 * @brief operator << : stream operator
 * @param stream std::ostream &
 * @param rhs Thread const&
 * @return std::ostream &
 */
std::ostream &operator<<(std::ostream &stream,const Thread &thrd)
{
    CRITICALOP(thrd);
    stream << "{Thread:" << &thrd << ",task:" << thrd.getTask() << ",type:" << thrd.getWorker()->getType();
    stream << ",term:" << (thrd.isTerminated()?'y':'n');
    stream << ",runs:" << (thrd.isStarted()?'y':'n');
    if (!!thrd.m_thread.get()) {
        stream << ",id:" << thrd.getBoostThread().get_id() << "}";
    } else {
        stream << ",id:0x00}";
    }
    return stream;
}

std::recursive_mutex Thread::m_critical_section;

} // namespace drumlin
