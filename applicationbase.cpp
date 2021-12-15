#define TAOJSON
#include "applicationbase.h"

#include <memory>
#include "application.h"
#include "thread.h"
#include "thread_worker.h"

namespace drumlin {

void ApplicationBase::getStatus(json::value *status)const
{
    THREADSLOCK
    json::value array(json::empty_array);
    for(threads_type::value_type const& thread : m_threads){
        json::value obj(json::empty_object);
        thread->getWorker()->writeToObject(&obj);//report the thread
        array.get_array().push_back(obj);

        thread->getWorker()->getStatus(status);//report sub-system
    }
    status->get_object().insert({std::string("threads"),array});
}

} // namespace drumlin
