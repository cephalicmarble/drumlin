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
            publish((HeapBuffer*)heap.second);
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
    std::vector<buffers_heap_map_type::value_type> pairs;
    int n = 0;
    std::for_each(m_buffers.begin(), m_buffers.end(),
        [&n, &pairs, use]
        (buffers_heap_map_type::value_type & pair) {
            if(use == pair.first) {
                n += pair.second->freeAll();
                pairs.push_back(pair);
            }
        });
    for(auto it : pairs)
    {
        m_buffers.erase(std::remove(m_buffers.begin(), m_buffers.end(), it), m_buffers.end());
    }
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
    std::for_each(m_buffers.begin(), m_buffers.end(),
        [&relevant, use](auto & buffer){
            {LOGLOCK;Debug() << "Cache" << __func__ << buffer.first;}
            if(use == buffer.first) {
                for(auto & block : buffer.second->blocks) {
                    relevant.push_back((HeapBuffer*)block.second);
                }
            }
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
    m_subscriptions.push_back(sub);
    return findRelevant(sub.first);
}

int BufferCache::unsubscribe(subs_map_type::value_type::second_type &acceptor)
{
    int n = 0;
    std::vector<subs_map_type::iterator> pairs;
    for(auto it(m_subscriptions.begin()); it != m_subscriptions.end(); ++it) {
        if (it->second == acceptor) {
            ++n;
            pairs.push_back(it);
        }
    }
    for(auto & p : pairs) {
        m_subscriptions.erase(p);
    }
    return n;
}

/**
 * @brief BufferCache::callSubscribed : loop over subscriptions to find relevant transforms
 * @param buffer Buffer
 * @param flush bool
 * @return guint32 number of transforms called
 */
guint32 BufferCache::publish(HeapBuffer * buffer)
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
getCacheStatus_t getCacheStatus(&BufferCache::getStatus);

BufferCache access::s_cache;
Allocator access::s_allocator;

} // namespace Buffers

} // namespace drumlin
