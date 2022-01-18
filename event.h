#ifndef EVENT_H
#define EVENT_H

#include <memory>
#include <string>
#include <functional>
using namespace std;
#include "drumlin.h"
#include "metatypes.h"
#include "object.h"
#include "thread.h"
#include "exception.h"

#define DrumlinEventTypes (\
    DrumlinEventNone,\
    DrumlinEventEvent_first,\
    DrumlinEventThread_first,\
    DrumlinEventThreadWork,\
    DrumlinEventThreadNotify,\
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

#define quietDebug() if(!quietEvents)Debug()

/**
 * @brief The Event class
 */
class Event
{
public:
    typedef gremlin::DrumlinEventType Type;
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

    Event();
    Event(Type _type);
    Event(Type _type,string _string);
    Event(Type _type,string _string,void *_pointer);
    Event(Event const& rhs);
    Event(Event && rhs);
    void init();
    virtual ~Event();

    friend ostream &operator <<(ostream &stream, const Event &event);
    friend logger &operator <<(logger &stream, const Event &event);

    template <typename T>
    T *getPointerVal() const
    {
        return dynamic_cast<T*>(static_cast<T*>(m_ptr));
    }

    template <typename T>
    T getVal() const
    {
        return static_cast<T>(m_ptr);
    }
private:
    Type m_type;
    string m_string;
    void *m_ptr;
};

namespace event {

std::shared_ptr<Event> make_event(Event::Type _type,const char *error,void *that = 0);
std::shared_ptr<Event> make_event(Event::Type _type,std::string const&,void *that = 0);

void punt(std::shared_ptr<Event>);

void send(Thread *target, std::shared_ptr<Event>);

void post(ThreadWorker *worker, std::shared_ptr<Event> event);

}

} // namespace drumlin

#endif // ERROR_H
