#ifndef _THREAD_ACCESSOR_H
#define _THREAD_ACCESSOR_H

#include <memory>
#include "application.h"
#include "thread.h"
#include "thread_worker.h"

namespace drumlin {

struct ThreadAccessFunctor {
    virtual ~ThreadAccessFunctor(){;}
    virtual void operator()(Thread *)const=0;
};

struct SendEvent
    : public ThreadAccessFunctor
{
    std::shared_ptr<Event> m_event;
    SendEvent(std::shared_ptr<Event> pevent);
    void operator()(Thread *_thread) const;
};

struct StartThread
    : public ThreadAccessFunctor
{
    StartThread();
    void operator()(Thread *_thread) const;
};

class ThreadAccessor {
    threads_type namedThreads;
    threads_type typedThreads;
    bool flagAllName;
    bool flagAllType;
    std::string m_name;
    std::string m_type;
    ThreadWorker const* m_worker;
    threads_type selection;
    std::lock_guard<std::recursive_mutex> m_mutex_lock;
public:
    ThreadAccessor();
    ThreadAccessor& clear();
    void allNames(bool b) { flagAllName = b; }
    void allTypes(bool b) { flagAllType = b; }
    /**
     * @brief ThreadAccessor::named : find threads of name or "all"
     * @param name string maybe "all"
     * @return bool !empty
     */
    ThreadAccessor& named(const string &name);
    /**
     * @brief ThreadAccessor::typed : find threads of type or "all"
     * @param type ThreadWorker::ThreadType maybe "all"
     * @return bool !empty
     */
    ThreadAccessor& typed(ThreadWorker::Type type);
    /**
     * @brief ThreadAccessor::selectBoth : intersect named and typed
     * @return bool !empty
     */
    ThreadAccessor& selectBoth();
    /**
     * @brief ThreadAccessor::selectNamed : get Named threads
     * @return bool !empty
     */
    ThreadAccessor& getNamed();
    /**
     * @brief ThreadAccessor::selectTyped : get Typed threads
     * @return bool !empty
     */
    ThreadAccessor& getTyped();
    /**
     * @brief ThreadAccessor::selectWorked : get thread for ThreadWorker
     * @return bool !empty
     */
    ThreadAccessor& getWorkered(ThreadWorker const*);

    /**
     * @brief ThreadAccessor::operator() : apply a ThreadAccessFunctor to selection
     * @param ThreadAccessFunctor const&
     * @return bool !empty
     */
    void operator()(ThreadAccessFunctor const& functor, bool throwIfEmpty = true);

    /**
     * @return bool !empty
     */
    bool selectionEmpty();

    /**
     * @brief ThreadAccessor::first() : retrieve the first of selection
     * @return Thread* selection[0]
     */
    threads_type::value_type first();

}; // class ThreadAccessor

} // drumlin

#endif // _THREAD_ACCESSOR_H
