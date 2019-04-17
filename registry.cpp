#include "registry.h"

#include "thread.h"
#include "source.h"
#include <boost/thread/lock_guard.hpp>

namespace drumlin {

template <>
void Registry<WorkObject>::remove(const std::string &str,bool noDelete)
{
    std::lock_guard<std::recursive_mutex> l(mutex);
    typename map_type::iterator it(map.find(str));
    if(it!=map.end()){
        (*it).second->stop();
        WorkObject *ptr(it->second);
        map.erase(it);
        if(!noDelete)
            delete ptr;
    }
}

template class Registry<drumlin::ThreadWorker>;

} // namespace drumlin
