#ifndef _BUFFER_CACHE_H
#define _BUFFER_CACHE_H

#include <list>
#include <memory>
#include <vector>
#include <deque>
#include <utility>
#include <unordered_map>
#include "tao_forward.h"
using namespace tao;
#include "buffer.h"

namespace drumlin {

namespace Buffers {

/**
 * @brief buffer_list_type :
 */
typedef std::list<std::pair<UseIdent, heap_ptr_type>> buffers_heap_map_type;
/**
 * @brief buffer_vec_type :
 */
typedef std::list<HeapBuffer*> buffer_list_type;
/**
 * @brief subs_map_type : relates a Buffers::Acceptor to a UseIdent
 */
typedef std::list<std::pair<UseIdentFilter,Acceptor*>> subs_map_type;

std::pair<const UseIdent,Buffers::Acceptor*> make_sub(const UseIdent &rel,Acceptor *a);

/**
 * @brief The BufferCache class : is our buffer heap
 */
class BufferCache
{
    static BufferCache cache;
    BufferCache();
protected:
    buffers_heap_map_type m_buffers;
    subs_map_type m_subscriptions;
public:
    std::mutex m_mutex;
    ~BufferCache();
    bool isLocked();

    /**
     * @brief addBuffers will increase HeapBuffer use_count
     * for all heap_t allocated exposed here, keyed by a UseIdent.
     */
    guint32 addAllocated(buffers_heap_map_type::value_type const&);
    guint32 clearAllocated(UseIdentFilter);
    buffer_list_type findRelevant(UseIdentFilter);
    buffer_list_type subscribe(subs_map_type::value_type);
    int unsubscribe(subs_map_type::value_type::second_type&);
    guint32 publish(HeapBuffer* buffer);
    int getStatus(json::value *status);
    friend struct access;
};
/*
 * thread-safe calls
 */
typedef mutex_call_1<BufferCache,guint32,buffers_heap_map_type::value_type const&> addAllocated_t;
typedef mutex_call_1<BufferCache,guint32,UseIdentFilter> clearAllocated_t;
typedef mutex_call_1<BufferCache,buffer_list_type,UseIdentFilter> findRelevant_t;
typedef mutex_call_1<BufferCache,buffer_list_type,subs_map_type::value_type> subscribe_t;
typedef mutex_call_1<BufferCache,int,subs_map_type::value_type::second_type&> unsubscribe_t;
typedef mutex_call_1<BufferCache,guint32,HeapBuffer*> publish_t;
typedef mutex_call_1<BufferCache,int,json::value *> getCacheStatus_t;

extern addAllocated_t addAllocated;
extern clearAllocated_t clearAllocated;
extern findRelevant_t findRelevant;
extern subscribe_t subscribe;
extern unsubscribe_t unsubscribe;
extern publish_t publish;
extern getCacheStatus_t getCacheStatus;

struct access
{
    /**
     * @brief Cache : thread-safe receiver for BufferCache
     * template<class CPS>
     * @return CPS::Return
     */
    template <class CPS>
    static typename CPS::Return cache(CPS cps)
    {
        while(!s_cache.m_mutex.try_lock()){
            boost::this_thread::yield();
        }
        typename CPS::Return ret(cps(&s_cache));
        s_cache.m_mutex.unlock();
        return ret;
    }

    template <class MemFun>
    static typename MemFun::result_type cache(MemFun fun,typename MemFun::second_argument_type &arg)
    {
        while(!s_cache.m_mutex.try_lock()){
            boost::this_thread::yield();
        }
        typename MemFun::result_type ret(fun(cache,arg));
        s_cache.m_mutex.unlock();
        return ret;
    }

    /**
     * @brief Allocator : thread-safety
     * template<class CPS>
     * @return CPS::Return
     */
    template <class CPS>
    typename CPS::Return allocator(CPS cps)
    {
        while(!s_allocator.m_mutex.try_lock()){
            boost::this_thread::yield();
        }
        typename CPS::Return ret(cps(&s_allocator));
        s_allocator.m_mutex.unlock();
        return ret;
    }

    static BufferCache s_cache;
    static Allocator s_allocator;
};

} // namespace Buffers

} // namespace drumlin

#endif // _BUFFER_CACHE_H
