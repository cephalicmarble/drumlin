#ifndef __ALLOCATOR_H
#define __ALLOCATOR_H

#include <memory>
#include <list>
#include <unordered_map>
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
    void *memory;
    void *_free;
    size_t align;
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
    int freeAll();
    void toJson(json::value *status);
};
typedef UsesAllocator* heap_key_type;
typedef std::shared_ptr<heap_t> heap_value_type;
typedef std::unordered_map<heap_key_type, heap_value_type> heap_map_type;
typedef std::shared_ptr<heap_t> heap_ptr_type;

/**
 * @brief The Allocator class : represents chunks of memory
 */
class Allocator
{
protected:
    heap_map_type m_heaps;
    int do_unregisterHeap(heap_key_type, heap_value_type second);
public:
    std::recursive_mutex m_mutex;
    Allocator();
    ~Allocator();
    heap_ptr_type registerUse(std::pair<heap_key_type, guint32> source);
    int unregisterUse(heap_key_type source);
    int unregisterAll(int);
    heap_ptr_type getHeap(heap_key_type source);
    int getStatus(json::value *status);
};

typedef mutex_call_1<Allocator, heap_ptr_type,  std::pair<heap_key_type, guint32>>  registerUse_t;
typedef mutex_call_1<Allocator, int,            heap_key_type>                      unregisterUse_t;
typedef mutex_call_1<Allocator, int,            int>                                unregisterAll_t;
typedef mutex_call_1<Allocator, heap_ptr_type,  heap_key_type>                      getHeap_t;
typedef mutex_call_1<Allocator, int,            json::value*>                       getAllocatorStatus_t;

extern registerUse_t registerUse;
extern unregisterUse_t unregisterUse;
extern unregisterAll_t unregisterAll;
extern getHeap_t getHeap;
extern getAllocatorStatus_t getAllocatorStatus;

extern Allocator allocator;

} // namespace Buffers

} // namespace drumlin

#endif // __ALLOCATOR_H
