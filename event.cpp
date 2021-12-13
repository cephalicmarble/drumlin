#include "event.h"

#include "drumlin.h"
#include "exception.h"
#include "thread.h"
#include "application.h"
#include "thread_accessor.h"

namespace drumlin {

bool quietEvents = false;

void setQuietEvents(bool keepQuiet)
{
    quietEvents = keepQuiet;
}

/**
 * @brief operator << : qDebug stream operator
 * @param stream QDebug
 * @param event Event
 * @return QDebug
 */
logger &operator <<(logger &stream, const Event &event)
{
    stream << event.getName();
    return stream;
}

/**
 * @brief Event::send : send the event to a thread queue
 * @param thread Thread*
 */
void Event::send(Thread *target)const
{
    target->post(const_cast<Event*>(this));
}

/**
 * @brief Event::punt : punt the event to the application queue
 */
void Event::punt()const
{
    drumlin::iapp->post(const_cast<Event*>(this));
}

/**
 * @brief Event::send : send the event to a thread queue
 * @param thread Thread*
 */
void Event::post() const
{
    Object *receiver(getPointerVal<Object>());
    ThreadWorker *worker(dynamic_cast<ThreadWorker*>(receiver));
    if(worker){
        ThreadAccessor access;
        access.getWorkered(worker);
        if(access.selectionEmpty()) {
            Debug() << "no thread for " << worker->getType();
        }else{
            Debug() << "sending" << getName() << "to" << access.first()->getName();
        }
        access(SendEvent(const_cast<Event*>(this)));
    }else{
        Debug() << "sending" << getName() << "to" << drumlin::iapp->getThreadId();
        drumlin::iapp->post(const_cast<Event*>(static_cast<const Event*>(this)));
    }
}

/**
 * @brief event_cast : used to stash something in an event for sending to eventFilter
 * @param _type DrumlinEventType
 * @param error const char*
 * @param that T*
 */
const Event *make_event(Event::Type _type,const char *error,void *that)
{
    return new Event(_type,error,that);
}

} // namespace drumlin
