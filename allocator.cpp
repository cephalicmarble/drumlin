#define TAOJSON
#include "allocator.h"

#include "drumlin.h"
#include "exception.h"
#include "uses_allocator.h"
#include "use_ident.h"
#include "buffer.h"
#include "buffer_cache.h"
#include "mutexcall.h"

namespace drumlin {

namespace Buffers {

/**
 * @brief heap_t::heap_t : prepare a heap
 * @param _total int
 * @param _size size_t
 * @param _memory void*
 * @param _align size_t
 */
heap_t::heap_t(int _total,size_t _size,void *_memory,void *__free,size_t _align,int _max)
    :allocated(0),total(_total),size(_size),memory(_memory),_free(__free),align(_align),max(_max)
{
    CPLATE;
    {LOGLOCK;Debug() << "HEAP" << total << "of" << size;}
}

/**
 * @brief heap_t::alloc : allocate a previously agreed upon sized chunk from this heap
 *
 * The heap keeps a list of marked pairs of pointers. The second is a pointer to an aligned
 * block of previously agreed upon size. The first is nullptr when not allocated (freed),
 * 1 when reallocated, and ==second when first allocated.
 *
 * @return char*
 */
byte *heap_t::alloc()
{
    byte *ptr,*alptr = nullptr;
    auto it(std::find_if(blocks.begin(),blocks.end(),[&alptr](auto &pair){
        if(pair.second != (byte*)-1) {
            if(pair.first == nullptr) {
                return true;
            }else{
                alptr = pair.second;
            }
        }
        return false;
    }));
    if(it != blocks.end()){ // for reuse
        {LOGLOCK;Debug() << "REALLOCATED" << (void*)(*it).first << ":" << (void*)(*it).second;}
        (*it).first = (byte*)1;
        allocated++;
        return (*it).second;
    }
    if(!allocated) {
        ptr = (byte*)memory;
    } else {
        ptr = blocks.back().second + size;
    }
    do{
        size_t remaining = max - std::distance((byte*)memory, blocks.back().second + size);
        if(remaining <= 2 * size){
            Critical() << "Heap full!";
        }
        allocated++;
        void *pv((void*)ptr);
        alptr = (byte*)std::align(align,size,pv,remaining);
        if(alptr){
            blocks.push_back({ ptr, alptr });
            if (drumlin::debug) {
                {LOGLOCK;Debug() << "ALLOCATED" << (void*)ptr << ":" << (void*)alptr;}
                if(drumlin::debug)
                {
                    for(auto & pair : blocks)
                    {
                        {LOGLOCK;Debug() << (void*)pair.first << ":" << (void*)pair.second;}
                    }
                }
            }
            return alptr;
        }
        return nullptr;
    }while(allocated<total);
    Critical() << "Heap overallocated!";
    return nullptr;
}

/**
 * @brief heap_t::free : free a block by marking its pair
 * @param block char*
 */
void heap_t::free(byte *block)
{
    array_t::iterator it(std::find_if(blocks.begin(),blocks.end(),[block](array_t::value_type &pair){
        return pair.second == (byte*)block && pair.first != nullptr;
    }));
    if(it == blocks.end()){
        if (drumlin::debug)
        {
            for(array_t::value_type &pair : blocks){
                {LOGLOCK;Debug() << (void*)pair.first << ":" << (void*)pair.second;}
            }
        }
        Critical() << "Double free?";
        goto returning;
    }
    allocated--;
    {LOGLOCK;Debug() << "FREED" << (void*)(*it).first << ":" << (void*)(*it).second;}
    (*it).first = nullptr;
returning:
    if (drumlin::debug)
    {
        for(array_t::value_type &pair : blocks){
            {LOGLOCK;Debug() << (void*)pair.first << ":" << (void*)pair.second;}
        }
    }
}

heap_t::~heap_t()
{
    DPLATE;
    ::free(_free);
}

void heap_t::toJson(json::value *heap)
{
    json::object_t &obj(heap->get_object());

    obj.insert({std::string("allocated"),allocated});
    obj.insert({std::string("total"),total});
    obj.insert({std::string("size"),size});
    obj.insert({std::string("align"),align});
}

int Allocator::getStatus(json::value *status)
{
    json::value allocator(json::empty_object);
    json::object_t &obj(allocator.get_object());

    json::value _heaps(json::empty_array);
    json::array_t &array(_heaps.get_array());
    for(auto const& heap : m_heaps){
        json::value _heap(json::empty_object);
        _heap.get_object().insert({std::string("token"),heap.first.lock().get()->getToken()});
        heap.second->toJson(&_heap);
        array.push_back(_heap);
    }
    obj.insert({"heaps",_heaps});
    status->get_object().insert({"allocator",allocator});
    return 0;
}

/**
 * @brief Allocator::Allocator : only constructor
 */
Allocator::Allocator()
{
    CPLATE;
}

/**
 * @brief Allocator::~Allocator : deletes buffers from the container, without flushing
 */
Allocator::~Allocator()
{
    DPLATE;

}

/**
 * @brief Allocator::registerSource : register a heap for the source, detailed by the source.
 * @param source Sources::Source*
 * @return int total size of allocated heap memory
 */
heap_ptr_type Allocator::registerUse(heap_key_type &usesAllocator, size_t alignment)
{
    size_t sz(usesAllocator.lock()->getDataLength());
    size_t n((usesAllocator.lock()->getQueueSize()));
    size_t m((sz + 8) * n),tmp = ceil(m / (1<<PAGE_SHIFT));
    m = (1 + tmp) * (1<<PAGE_SHIFT);
//    size_t size(m);
    void *_free(malloc(m));
    if(!_free)
        throw Exception("ENOMEM");
    void *mem(std::align(alignment,sz,_free,m));
    if(!mem)
        throw Exception("ENOALIGN");

    heap_t *pHeap(new heap_t(n - 1,sz,mem,_free,alignment,m));
    auto entry = std::make_pair(heap_key_type(usesAllocator),heap_value_type(pHeap));
    m_heaps.insert(entry);

    return entry.second;
}

int Allocator::do_unregisterHeap(heap_map_type::value_type pair)
{
    if(pair.second->allocated){
        UseIdent use(pair.first);
        drumlin::Buffers::access::cache(CPS_call_void(Buffers::clearAllocated, UseIdentFilter(SourcesOnly(use))));
    }
    try {
        ::free(pair.second->_free);
        int freud(pair.second->total * pair.second->size);
        m_heaps.erase(pair.first);
        pair.second.reset();
        return freud;
    } catch (Exception &e) {
        {LOGLOCK;Debug() << __func__ << e;}
    } catch (std::exception &e) {
        {LOGLOCK;Debug() << __func__ << e;}
    }
}

/**
 * @brief Allocator::unregisterSource : *DANGEROUS* free the heap, first calling Cache(clearRelevantBuffers)
 * @param source Sources::Source*
 * @return int
 */
int Allocator::unregisterUse(heap_key_type &source)
{
    return do_unregisterHeap(m_heaps.find(source));
}

/**
 * @brief Allocator::unregisterSources : free the heap, first calling Cache(clearRelevantBuffers)
 * @return int
 */
int Allocator::unregisterAll()
{
    int freud(0);
    std::for_each(m_heaps.begin(),m_heaps.end(),[&freud,this](buffer_heap_type::value_type &pair){
        freud += do_unregisterHeap(pair);
    });
    return freud;
}

/**
 * @brief Allocator::unregisterSource : fetch the heap
 * @param source Sources::Source*
 * @return int
 */
heap_ptr_type Allocator::getHeap(std::weak_ptr<UsesAllocator> uses)
{
    auto it = m_heaps.find(uses);
    if(m_heaps.end() != it)
        return it->second;
    return nullptr;
}

/**
 * @brief Allocator::free : free a chunk from the source's heap
 * @param args
 * @return
 */
int Allocator::free(HeapBuffer *buffer)
{
    std::weak_ptr<UsesAllocator> use(buffer->getUseIdent().getUse());
    if(m_heaps.end()==m_heaps.find(use))
        return 0;
    heap_ptr_type heap(m_heaps.at(use));
    return heap->free(buffer);
}

registerSource_t registerSource(&Allocator::registerSource);
unregisterSource_t unregisterSource(&Allocator::unregisterSource);
unregisterSources_t unregisterSources(&Allocator::unregisterSources);
getHeap_t getHeap(&Allocator::getHeap);
alloc_t alloc(&Allocator::alloc);
free_t free(&Allocator::free);
getAllocatorStatus_t getAllocatorStatus(&Allocator::getStatus);

Pleg::Buffers::Allocator allocator;

} // namespace Buffers

} // namespace drumlin
