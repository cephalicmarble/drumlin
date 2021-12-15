#include "thread_accessor.h"

#include <algorithm>

SendEvent::SendEvent(std::shared_ptr<Event> pevent) : m_event(pevent)
{

}

void SendEvent::operator()(Thread *_thread) const
{
    event::send(_thread, m_event);
}

StartThread::StartThread() {;}

void StartThread::operator()(Thread *_thread) const
{
    if(!_thread->isStarted()) _thread->start();
}

ThreadAccessor::ThreadAccessor(): m_mutex_lock(const_cast<std::recursive_mutex&>(iapp->m_critical_section))
{
    clear();
}

ThreadAccessor& ThreadAccessor::clear()
{
    selection.clear();
    flagAllName = false;
    flagAllType = false;
    m_name = "";
    m_type = "";
    m_worker = nullptr;
    return *this;
}

/**
 * @brief ThreadAccessor::named : find threads of name or "all"
 * @param name string maybe "all"
 * @return bool
 */
ThreadAccessor& ThreadAccessor::named(const string &name)
{
    // FRIENDTHREADSLOCK
    namedThreads.clear();
    flagAllName = false;

    std::copy_if(
        iapp->m_threads.begin(),
        iapp->m_threads.end(),
        std::back_inserter(namedThreads),
        [name] (auto const& thread) {
            return thread->getTask() == name;
        });
    m_name = name;
    return *this;
}

/**
 * @brief ThreadAccessor::typed : find threads of type or "all"
 * @param type ThreadWorker::ThreadType maybe "all"
 * @return std::vector<Thread*>
 */
ThreadAccessor& ThreadAccessor::typed(ThreadWorker::Type type)
{
    // FRIENDTHREADSLOCK
    typedThreads.clear();
    flagAllType = false;
    std::copy_if(
        iapp->m_threads.begin(),
        iapp->m_threads.end(),
        std::back_inserter(typedThreads),
        [type] (auto const& thread) {
            return thread->getWorker()->getType() == type;
        });
    m_type = gremlin::metaEnum<ThreadType>().toString(type);
    return *this;
}

/**
 * @brief ThreadAccessor::selectBoth : intersect named and typed
 * @return
 */
ThreadAccessor& ThreadAccessor::selectBoth()
{
    // FRIENDTHREADSLOCK
    selection.clear();
    if (flagAllName && flagAllType) {
        std::copy(
            iapp->m_threads.begin(),
            iapp->m_threads.end(),
            std::back_inserter(selection));
        return *this;
    }
    threads_type::iterator beginNames(flagAllName ?
        namedThreads.begin() :
        iapp->m_threads.begin());
    threads_type::iterator endNames(flagAllName ?
        namedThreads.end() :
        iapp->m_threads.end());
    threads_type::iterator beginTypes(flagAllType ?
        typedThreads.begin() :
        iapp->m_threads.begin());
    threads_type::iterator endTypes(flagAllName ?
        namedThreads.begin() :
        iapp->m_threads.begin());

    std::set_intersection(
        beginNames, endNames, beginTypes, endTypes,
        std::back_inserter(selection)
    );
    return *this;
}

/**
 * @brief ThreadAccessor::selectNamed : get Named threads
 * @return
 */
ThreadAccessor& ThreadAccessor::getNamed()
{
    selection.clear();
    if (flagAllName) {
        // FRIENDTHREADSLOCK
        std::copy(
            iapp->m_threads.begin(),
            iapp->m_threads.end(),
            std::back_inserter(selection));
        return *this;
    }
    std::copy(
        namedThreads.begin(),
        namedThreads.end(),
        std::back_inserter(selection));
    return *this;
}


/**
 * @brief ThreadAccessor::selectTyped : get Typed threads
 * @return
 */
ThreadAccessor& ThreadAccessor::getTyped()
{
    selection.clear();
    if (flagAllType) {
        // FRIENDTHREADSLOCK
        std::copy(
            iapp->m_threads.begin(),
            iapp->m_threads.end(),
            std::back_inserter(selection));
        return *this;
    }
    std::copy(
        typedThreads.begin(),
        typedThreads.end(),
        std::back_inserter(selection));
    return *this;
}

/**
 * @brief ThreadAccessor::selectWorked : get thread for ThreadWorker
 * @return
 */
ThreadAccessor& ThreadAccessor::getWorkered(ThreadWorker const* worker)
{
    // FRIENDTHREADSLOCK
    selection.clear();
    std::copy_if(
        iapp->m_threads.begin(),
        iapp->m_threads.end(),
        std::back_inserter(selection),
        [worker] (auto const& thread) {
            return thread->getWorker().get() == worker;
        });
    m_worker = worker;
    return *this;
}

/**
 * @brief Application::operator() : apply a ThreadAccessFunctor to selection
 * @param ThreadAccessFunctor const&
 */
void ThreadAccessor::operator()(ThreadAccessFunctor const& functor, bool throwIfEmpty)
{
    if (throwIfEmpty && selectionEmpty()) {
        std::stringstream ss;
        ss << __FILE__ << "accessor empty";
        if (0 != m_name.length()) {
            ss << " named:" << m_name;
        }
        if (0 != m_type.length()) {
            ss << " typed:" << m_type;
        }
        if (nullptr != m_worker) {
            ss << " worker:" << m_worker;
        }
        throw Exception(ss);
    }
    std::for_each(selection.begin(), selection.end(), [&functor](auto thread){
        functor(thread);
    });
}

/**
 * @return bool !empty
 */
bool ThreadAccessor::selectionEmpty()
{
    return selection.empty();
}

/**
 * @brief ThreadAccessor::first() : retrieve the first of selection
 * @return Thread* selection[0]
 */
threads_type::value_type ThreadAccessor::first()
{
    if (selectionEmpty()) {
        throw Exception("Selection is empty!");
    }
    return selection[0];
}
