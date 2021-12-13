#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <functional>
using namespace std;
#include "drumlin.h"
#include "metatypes.h"
#include "object.h"
#include "thread.h"
#include "exception.h"
#include "applicationbase.h"

#define DrumlinEventTypes (\
    DrumlinEventNone,\
    DrumlinEventEvent_first,\
    DrumlinEventThread_first,\
    DrumlinEventThreadWork,\
    DrumlinEventThreadWarning,\
    DrumlinEventThreadShutdown,\
    DrumlinEventThreadRemove,\
    DrumlinEventThread_last,\
\
    DrumlinEventApplicationClose,\
    DrumlinEventApplicationThreadsGone,\
    DrumlinEventApplicationShutdown,\
    DrumlinEventApplicationRestart,\
    DrumlinEventEvent_last\
)
ENUM(DrumlinEventType,DrumlinEventTypes)

namespace drumlin {

class Thread;

extern bool quietEvents;
#define quietDebug() if(!quietEvents)Debug()
void setQuietEvents(bool keepQuiet);

/**
 * @brief The Event class
 */
class Event
{
public:
    typedef guint32 Type;
public:
    /**
     * @brief type
     * @return DrumlinEventType
     */
    virtual Type type()const{ return m_type; }
    /**
     * @brief getName
     * @return string
     */
    string getName()const{ return m_string; }
    /**
     * @brief Event : empty constructor
     */
    Event()
    :m_type(0),m_string("none"),m_ptr(nullptr) {}
    Event(Type _type)
    :m_type(_type),m_string(""),m_ptr(nullptr) {}
    Event(Type _type,string _string)
    :m_type(_type),m_string(_string),m_ptr(nullptr) {}
    Event(Type _type,string _string,void *_pointer)
    :m_type(_type),m_string(_string),m_ptr(_pointer) {}
    /**
     * @brief clone : used by thread event filter
     * @return Event*
     */
    virtual Event *clone()
    {
        return new Event(type(),getName());
    }
    /**
     * @brief Event::~Event
     */
    virtual ~Event(){}
    friend ostream &operator <<(ostream &stream, const Event &event);
    friend logger &operator <<(logger &stream, const Event &event);
    void punt()const;
    void send(Thread *target)const;

    template <typename T>
    T *getPointerVal() const
    {
        T *pt(dynamic_cast<T*>(static_cast<T*>(m_ptr)));
        if (pt == nullptr) {
            std::stringstream ss;
            ss << m_type << " event (" << m_string << ") fail cast to pointer:" << m_ptr;
            throw Exception(ss);
        }
        return pt;
    }

    template <typename T>
    T getVal() const
    {
        return static_cast<T>(m_ptr);
    }

    /**
     * @brief Event::send : send the event to a thread queue
     * @param thread Thread*
     */
    void post()const;
private:
    Type m_type;
    string m_string;
    void *m_ptr;
};

const Event *make_event(Event::Type _type,const char *error,void *that = 0);

} // namespace drumlin

#endif // ERROR_H
