#include "event.h"

#include "drumlin.h"
#include "exception.h"
#include "thread.h"
#include "application.h"
#include "thread_accessor.h"

namespace drumlin {

/**
 * @brief operator << : qDebug stream operator
 * @param stream QDebug
 * @param event Event
 * @return QDebug
 */
logger &operator <<(logger &stream, const Event &event)
{
    std::stringstream ss;
    ss << "{Event:" << event.getName() << ":" << metaEnum<DrumlinEventType>().toString((DrumlinEventType)event.type()) << "}";
    stream << ss.str();
    return stream;
}

Event::Event()
:m_type(0),m_string("none"),m_ptr(nullptr)
{
    init();
}

Event::Event(Type _type)
:m_type(_type),m_string(""),m_ptr(nullptr)
{
    init();
}

Event::Event(Type _type,string _string)
:m_type(_type),m_string(_string),m_ptr(nullptr)
{
    init();
}

Event::Event(Type _type,string _string,void *_pointer)
:m_type(_type),m_string(_string),m_ptr(_pointer)
{
    init();
}

Event::Event(Event const& rhs)
:m_type(rhs.type()),m_string(rhs.getName()),m_ptr(rhs.getVal<void*>())
{
    Debug() << "copy Event" << rhs << " to " << *this << ":" << this;
}

Event::Event(Event && rhs)
:m_type(std::move(rhs.m_type)),
 m_string(std::move(rhs.m_string)),
 m_ptr(std::move(rhs.m_ptr))
{
    Debug() << "copy Event" << rhs << " to " << *this << ":" << this;
}


void Event::init() {
    CPLATE;
}
Event::~Event(){
    DPLATE;
}

namespace event {

/**
 * @brief event_cast : used to stash something in an event for sending to eventFilter
 * @param _type DrumlinEventType
 * @param error const char*
 * @param that T*
 */
std::shared_ptr<Event> make_event(Event::Type _type,const char *name,void *that)
{
    return std::shared_ptr<Event>(new Event(_type,name,that));
}

std::shared_ptr<Event> make_event(Event::Type _type,std::string const& name,void *that)
{
    return std::shared_ptr<Event>(new Event(_type,name.c_str(),that));
}

/**
 * @brief Event::punt : punt the event to the application queue
 */
void punt(std::shared_ptr<Event> event)
{
    drumlin::iapp->post(event);
}

/**
 * @brief Event::send : send the event to a thread queue
 * @param thread Thread*
 */
void send(Thread *target, std::shared_ptr<Event> event)
{
    target->post(event);
}

/**
 * @brief Event::send : send the event to a thread queue
 * @param thread Thread*
 */
void post(ThreadWorker *worker, std::shared_ptr<Event> event)
{
    ThreadAccessor access;
    access.getWorkered(worker);
    if(access.selectionEmpty()) {
        Debug() << "no thread for " << worker->getType();
        return;
    }
    Debug() << "sending" << *event << "to" << access.first()->getName();
    access(SendEvent(event));
}

} // namespace Event

} // namespace drumlin
