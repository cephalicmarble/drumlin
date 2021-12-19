#ifndef __ALLOCATOR_H
#define __ALLOCATOR_H

#include <memory>
#include <list>
#include <map>
#include <mutex>
#include <utility>
#include "tao_forward.h"
using namespace tao;
#include "mutexcall.h"
#include "gtypes.h"

namespace drumlin {

class UsesAllocator;
class HeapBuffer;

namespace Buffers {

#define PAGE_SHIFT 12UL

/**
 * @brief buffer_heap_type : where buffers go to die...
 */
struct heap_t {
    int allocated;
    int total;
    size_t size;
    size_t align;
    void *memory;
    void *_free;
    int max;
    typedef std::list<std::pair<byte*,byte*>> array_t;
    array_t blocks;
    heap_t(int _total,size_t _size,void *_memory,void *__free,size_t _align,int _max);
    ~heap_t();
    byte *alloc();
protected:
    byte *_align(void *ptr);
public:
    void free(byte *block);

    void toJson(json::value *status);
};
typedef std::weak_ptr<UsesAllocator> heap_key_type;
typedef std::shared_ptr<heap_t> heap_value_type;
typedef std::map<heap_key_type, heap_value_type> heap_map_type;
typedef std::shared_ptr<heap_t> heap_ptr_type;

/**
 * @brief The Allocator class : represents chunks of memory
 */
class Allocator
{
protected:
    heap_map_type m_heaps;
    int do_unregisterHeap(heap_map_type::value_type pair);
public:
    std::recursive_mutex m_mutex;
    Allocator();
    ~Allocator();
    heap_ptr_type registerUse(heap_key_type &source, size_t alignment);
    int unregisterUse(heap_key_type &source);
    int unregisterAll();
    heap_ptr_type getHeap(std::weak_ptr<UsesAllocator> source);
    int free(std::shared_ptr<HeapBuffer>);
    int getStatus(json::value *status);
};

typedef mutex_call_1<Allocator,int,UsesAllocator*> registerUse_t;
typedef mutex_call_1<Allocator,int,UsesAllocator*> unregisterUse_t;
typedef mutex_call_1<Allocator,int,int> unregisterAll_t;
typedef mutex_call_1<Allocator,const heap_t*,std::weak_ptr<UsesAllocator>> getHeap_t;
typedef mutex_call_1<Allocator,void*,std::shared_ptr<HeapBuffer>> alloc_t;
typedef mutex_call_1<Allocator,int,std::shared_ptr<HeapBuffer>> free_t;
typedef mutex_call_1<Allocator,int,json::value*> getAllocatorStatus_t;

extern registerUse_t registerUse;
extern unregisterUse_t unregisterUse;
extern unregisterAll_t unregisterAll;
extern getHeap_t getHeap;
extern alloc_t alloc;
extern free_t free;
extern getAllocatorStatus_t getAllocatorStatus;

extern Allocator allocator;

} // namespace Buffers

} // namespace drumlin

#endif // __ALLOCATOR_H
