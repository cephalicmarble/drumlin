#define TAOJSON
#include "applicationbase.h"

#include <memory>
#include "thread.h"
#include "thread_worker.h"

namespace drumlin {

void ApplicationBase::getStatus(json::value *status)const
{
    std::lock_guard<std::mutex> l(const_cast<std::mutex&>(m_critical_section));
    json::value array(json::empty_array);
    for(threads_type::value_type const& thread : threads){
        json::value obj(json::empty_object);
        Thread *_thread(thread);
        if(!_thread->isStarted() || _thread->isTerminated())
            continue;
        thread->getWorker()->writeToObject(&obj);//report the thread
        array.get_array().push_back(obj);
        thread->getWorker()->getStatus(status);//report sub-system
    }
    status->get_object().insert({"threads",array});
}

} // namespace drumlin
