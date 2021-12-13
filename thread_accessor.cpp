#include "thread_accessor.h"

#include <algorithm>

SendEvent::SendEvent(Event* event) {
    m_event.reset(event);
}

void SendEvent::operator()(Thread *_thread) const {
    _thread->post(m_event.get());
}

/**
 * @brief ThreadAccessor::named : find threads of name or "all"
 * @param name string maybe "all"
 * @return bool
 */
bool ThreadAccessor::named(const string &name)
{
    FRIENDTHREADSLOCK
    namedThreads.clear();
    flagAllName = false;
    std::copy_if(
        iapp->threads.begin(),
        iapp->threads.end(),
        std::back_inserter(namedThreads),
        [name] (auto const& thread) {
            return thread->getName() == name;
        });
    return !namedThreads.empty();
}

/**
 * @brief ThreadAccessor::typed : find threads of type or "all"
 * @param type ThreadWorker::ThreadType maybe "all"
 * @return std::vector<Thread*>
 */
bool ThreadAccessor::typed(ThreadWorker::Type type)
{
    FRIENDTHREADSLOCK
    typedThreads.clear();
    flagAllType = false;
    std::copy_if(
        iapp->threads.begin(),
        iapp->threads.end(),
        std::back_inserter(typedThreads),
        [type] (auto const& thread) {
            return thread->getWorker()->getType() == type;
        });
    return !namedThreads.empty();
}

/**
 * @brief ThreadAccessor::selectBoth : intersect named and typed
 * @return
 */
bool ThreadAccessor::selectBoth()
{
    FRIENDTHREADSLOCK
    selection.clear();
    if (flagAllName && flagAllType) {
        std::copy(
            iapp->threads.begin(),
            iapp->threads.end(),
            std::back_inserter(selection));
        return !selection.empty();
    }
    threads_type::iterator beginNames(flagAllName ?
        namedThreads.begin() :
        iapp->threads.begin());
    threads_type::iterator endNames(flagAllName ?
        namedThreads.end() :
        iapp->threads.end());
    threads_type::iterator beginTypes(flagAllType ?
        typedThreads.begin() :
        iapp->threads.begin());
    threads_type::iterator endTypes(flagAllName ?
        namedThreads.begin() :
        iapp->threads.begin());

    std::set_intersection(
        beginNames, endNames, beginTypes, endTypes,
        std::back_inserter(selection)
    );
    return !selection.empty();
}

/**
 * @brief ThreadAccessor::selectNamed : get Named threads
 * @return
 */
bool ThreadAccessor::getNamed()
{
    selection.clear();
    if (flagAllName) {
        FRIENDTHREADSLOCK
        std::copy(
            iapp->threads.begin(),
            iapp->threads.end(),
            std::back_inserter(selection));
        return !selection.empty();
    }
    std::copy(
        namedThreads.begin(),
        namedThreads.end(),
        std::back_inserter(selection));
    return !selection.empty();
}


/**
 * @brief ThreadAccessor::selectTyped : get Typed threads
 * @return
 */
bool ThreadAccessor::getTyped()
{
    selection.clear();
    if (flagAllType) {
        FRIENDTHREADSLOCK
        std::copy(
            iapp->threads.begin(),
            iapp->threads.end(),
            std::back_inserter(selection));
        return !selection.empty();
    }
    std::copy(
        typedThreads.begin(),
        typedThreads.end(),
        std::back_inserter(selection));
    return !selection.empty();
}

/**
 * @brief ThreadAccessor::selectWorked : get thread for ThreadWorker
 * @return
 */
bool ThreadAccessor::getWorkered(ThreadWorker const* worker)
{
    FRIENDTHREADSLOCK
    selection.clear();
    std::copy_if(
        iapp->threads.begin(),
        iapp->threads.end(),
        std::back_inserter(selection),
        [worker] (auto const& thread) {
            return thread->getWorker().get() == worker;
        });
    return !selection.empty();
}

/**
 * @brief Application::operator() : apply a ThreadAccessFunctor to selection
 * @param ThreadAccessFunctor const&
 */
void ThreadAccessor::operator()(ThreadAccessFunctor const& functor)
{
    for (auto thread : selection)
    {
        functor(thread);
    }
}

/**
 * @return bool !empty
 */
bool ThreadAccessor::selectionEmpty()
{
    return !selection.empty();
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
