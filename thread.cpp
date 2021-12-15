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
        INTERNAL;
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
    INTERNAL;
    return *m_thread;
}

/**
 * @brief getWorker
 * @return ThreadWorker*
 */
std::shared_ptr<ThreadWorker> Thread::getWorker()const
{
    INTERNAL;
    return m_worker;
}

/**
 * @brief hasWorker
 * @return bool
 */
bool Thread::hasWorker()const
{
    INTERNAL;
    return m_worker.get() != nullptr;
}

/**
 * @brief isStarted
 * @return bool
 */
bool Thread::isStarted()const
{
    INTERNAL;
    return m_ready;
}

/**
 * @brief Thread::getName : (string)"<type>:<task>"
 * @return string
 */
string Thread::getType()const
{
    INTERNAL;
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
    static std::mutex localMutex;
    std::lock_guard<std::mutex> l(const_cast<std::mutex&>(localMutex));

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
    CRITICAL;
    {
        INTERNAL;
        if (m_ready) return;
        m_ready = true;
        m_thread.reset(new boost::thread(&Thread::run,this));
    }
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
    if(!m_terminated) {
        static std::mutex localMutex;
        while(m_ready && !m_deleting && !m_terminated && !m_thread->interruption_requested()) {
            if(localMutex.try_lock())
            {
                exec();
            }
            localMutex.unlock();
            boost::this_thread::yield();
            boost::this_thread::sleep(boost::posix_time::milliseconds(100));
        }
        m_deleting = true;
    }
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
    m_queue.push(event);
}

void Thread::exec()
{
    {LOGLOCK;Debug() << "thread::exec" << *this;}
    queue_type::value_type pevent;
    try {
        boost::this_thread::interruption_point();
        {
            boost::this_thread::disable_interruption di;
            m_queue.wait_pull(pevent);
        }
        if(!!pevent) {
            if(!event(pevent))
                {LOGLOCK;Critical() << __func__ << "not handling event" << *pevent;}
        }
    } catch(thread_interrupted &ti) {
        THREADLOG2(*pevent, "thread_interrupted");
    } catch(Exception const& e) {
        THREADLOG2(*pevent, e);
    } catch(std::exception const& e) {
        THREADLOG2(*pevent, e);
    }
}

void Thread::quit()
{
    INTERNAL;
    //m_thread->interrupt();
    event::punt(event::make_event(DrumlinEventThreadRemove,"removeThread",this));
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
    //INTERNALOP(thrd);
    stream << "{Thread:" << &thrd << ",task:" << thrd.getTask() << ",type:" << thrd.getWorker()->getType();
    stream << ",term:" << (thrd.isTerminated()?'y':'n');
    stream << ",runs:" << (thrd.isStarted()?'y':'n');
    stream << ",del:" << (thrd.m_deleting?'y':'n');
    if (!!thrd.m_thread.get()) {
        stream << ",id:" << thrd.getBoostThread().get_id() << "}";
    } else {
        stream << ",id:0x00}";
    }
    return stream;
}

std::recursive_mutex Thread::m_critical_section;

} // namespace drumlin
