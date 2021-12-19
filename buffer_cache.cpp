#define TAOJSON
#include "buffer_cache.h"

#include <vector>
#include <algorithm>
#include <boost/date_time/posix_time/posix_time.hpp>
#include "drumlin.h"

namespace drumlin {

namespace Buffers {

/**
 * @brief BufferCache::BufferCache : only constructor
 */
BufferCache::BufferCache()
{
}

/**
 * @brief BufferCache::~BufferCache : deletes buffers from the container, without flushing
 */
BufferCache::~BufferCache()
{
    std::lock_guard<std::mutex> l(m_mutex);
}

/**
 * @brief BufferCache::isLocked : check the cache mutex
 * @return bool
 */
bool BufferCache::isLocked()
{
    if(m_mutex.try_lock()){
        m_mutex.unlock();
        return false;
    }
    return true;
}

/**
 * @brief BufferCache::addBuffer : adds buffer to the container
 * @param std::pair<UseIdent, heap_ptr_type>
 * @return quint32 number of buffers dealt
 */
guint32 BufferCache::addAllocated(buffers_heap_map_type::value_type const& pair)
{
    {LOGLOCK;Debug() << "Cache" << pair.first.getSourceName() << boost::posix_time::microsec_clock();}
    m_buffers.push_back(pair);
    int n = 0;
    std::for_each(pair.second->blocks.begin(), pair.second->blocks.end(),
        [this, &n](auto & heap){
            publish(heap.second);
            ++n;
        });
    return n;
}

// /**
//  * @brief BufferCache::flushDeadBuffers : loop within loop to find subscribed transforms and flush the dead buffers
//  * @return number of buffers removed
//  */
// guint32 BufferCache::flushDeadBuffers()
// {
//     guint32 c(0);
//     buffers.erase(std::remove_if(buffers.begin(),buffers.end(),[this,&c](buffers_type::value_type &buffer){
//         if(buffer->isDead()){
//             callSubscribed(buffer.get(),true);
//             c++;
//             return true;
//         }
//         return false;
//     }),buffers.end());
//     for(subs_type::value_type &sub : subscriptions){
//         sub.second->flush(nullptr);
//     }
//     return c;
// }

/**
 * @brief BufferCache::clearRelevantBuffers : loop to delete buffers by relevance
 * @return number of buffers removed
 */
guint32 BufferCache::clearAllocated(UseIdentFilter use)
{
    buffers_container_type::iterator it(std::find_if(m_buffers.begin(), m_buffers.end(),
        [use](buffers_container_type::value_type & pair){
            return use == pair.first;
        }));
    int n = 0;
    if(it != m_buffers.end()) {
        std::for_each(it->second->blocks.begin(), it->second->blocks.end(), [&n](heap_t::array_t::value_type &pair){
            ((HeapBuffer*)pair.second)->~HeapBuffer();
            pair.first = nullptr;
            ++n;
        });
    }
    m_buffers.erase(std::remove_if(m_buffers.begin(),m_buffers.end(),
        [use](buffers_container_type::value_type &pair){
            return use == pair.first;
        }),m_buffers.end());
    return n;
}

/**
 * @brief BufferCache::findRelevant : loop over buffers to find relevant entries
 * @param rel Relevance
 * @return Buffers::VectorOfBuffers*
 */
buffer_list_type BufferCache::findRelevant(UseIdentFilter use)
{
    buffer_list_type relevant;
    std::copy_if(m_buffers.begin(), m_buffers.end(), std::back_inserter(relevant),
        [use](buffers_container_type::value_type &buffer){
            {LOGLOCK;Debug() << "Cache" << __func__ << buffer.first;}
            return use == buffer.first;
        });
    return relevant;
}

/**
 * @brief BufferCache::subscribe
 * @param std::pair<UseIdent, Acceptor*> (see Buffers::make_sub)
 * @return Buffers::VectorOfBuffers* relevant
 */
buffer_list_type BufferCache::subscribe(subs_map_type::value_type sub)
{
    m_subscriptions.insert(sub);
    return findRelevant(sub.first);
}

int BufferCache::unsubscribe(subs_map_type::value_type::second_type &acceptor)
{
    int n = 0;
    m_subscriptions.erase(std::remove_if(m_subscriptions.begin(), m_subscriptions.end(),
        [acceptor, &n](auto & sub) {
            if (sub.second == acceptor) {
                ++n;
                return true;
            }
            return false;
        }));
    return n;
}

/**
 * @brief BufferCache::callSubscribed : loop over subscriptions to find relevant transforms
 * @param buffer Buffer
 * @param flush bool
 * @return guint32 number of transforms called
 */
guint32 BufferCache::publish(std::shared_ptr<HeapBuffer> buffer)
{
    guint32 c(0);
    for(subs_map_type::value_type &sub : m_subscriptions)
    {
        if(sub.first == buffer->getUseIdent()){
            sub.second->accept(buffer);
            c++;
        }
    }
    return c;
}

int BufferCache::getStatus(json::value *status)
{
    json::value cache(json::empty_object);
    json::object_t &obj(cache.get_object());

    json::value subs(json::empty_array);
    json::array_t &array(subs.get_array());
    for(subs_map_type::value_type const& sub : m_subscriptions){
        json::value _sub(json::empty_object);
        UseIdent(sub.first).toJson(&_sub);
        array.push_back(_sub);
    }
    obj.insert({"subs",subs});
    status->get_object().insert({"cache",cache});
    return 0;
}

/*
* thread-safe calls
*/
addAllocated_t addAllocated(&BufferCache::addAllocated);
clearAllocated_t clearAllocated(&BufferCache::clearAllocated);
findRelevant_t findRelevant(&BufferCache::findRelevant);
subscribe_t subscribe(&BufferCache::subscribe);
unsubscribe_t unsubscribe(&BufferCache::unsubscribe);
publish_t publish(&BufferCache::publish);
getStatus_t getStatus(&BufferCache::getStatus);

} // namespace Buffers

} // namespace drumlin
