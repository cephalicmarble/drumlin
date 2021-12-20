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

int heap_t::freeAll()
{
    int n = 0;
    std::for_each(blocks.begin(), blocks.end(), [&n](array_t::value_type &pair){
        if(pair.first != nullptr) {
            ++n;
            ((HeapBuffer*)pair.second)->~HeapBuffer();
            pair.first = nullptr;
        }
    });
    return n;
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
        _heap.get_object().insert({std::string("token"),heap.first->getToken()});
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
    APLATE;
}

/**
 * @brief Allocator::~Allocator : deletes buffers from the container, without flushing
 */
Allocator::~Allocator()
{
    BPLATE;
}

/**
 * @brief Allocator::registerSource : register a heap for the source, detailed by the source.
 * @param source Sources::Source*
 * @return int total size of allocated heap memory
 */
heap_ptr_type Allocator::registerUse(std::pair<heap_key_type, guint32> usesAlignment)
{
    size_t sz(usesAlignment.first->getDataLength());
    size_t n((usesAlignment.first->getQueueSize()));
    size_t m((sz + 8) * n),tmp = ceil(m / (1<<PAGE_SHIFT));
    m = (1 + tmp) * (1<<PAGE_SHIFT);
//    size_t size(m);
    void *_free(malloc(m));
    if(!_free)
        throw Exception("ENOMEM");
    void *mem(std::align(usesAlignment.second,sz,_free,m));
    if(!mem)
        throw Exception("ENOALIGN");

    heap_t *pHeap(new heap_t(n - 1,sz,mem,_free,usesAlignment.second,m));
    auto entry = std::make_pair(heap_key_type(usesAlignment.first),heap_value_type(pHeap));
    m_heaps.insert(entry);

    return entry.second;
}

int Allocator::do_unregisterHeap(heap_key_type first, heap_value_type second)
{
    if(second->allocated){
        UseIdent use(first);
        drumlin::Buffers::access::cache(CPS_call_void(Buffers::clearAllocated, UseIdentFilter(SourcesOnly(use))));
    }
    try {
        ::free(second->_free);
        int freud(second->total * second->size);
        m_heaps.erase(first);
        second.reset();
        return freud;
    } catch (Exception &e) {
        {LOGLOCK;Debug() << __func__ << e;}
    } catch (std::exception &e) {
        {LOGLOCK;Debug() << __func__ << e;}
    }
    return -1;
}

/**
 * @brief Allocator::unregisterSource : *DANGEROUS* free the heap, first calling Cache(clearRelevantBuffers)
 * @param source Sources::Source*
 * @return int
 */
int Allocator::unregisterUse(heap_key_type uses)
{
    auto it = m_heaps.find(uses);
    if (it != m_heaps.end()) {
        return do_unregisterHeap(it->first, it->second);
    }
    return -2;
}

/**
 * @brief Allocator::unregisterSources : free the heap, first calling Cache(clearRelevantBuffers)
 * @return int
 */
int Allocator::unregisterAll(int)
{
    int freud(0);
    std::for_each(m_heaps.begin(),m_heaps.end(),[&freud,this](auto &pair){
        freud += do_unregisterHeap(pair.first, pair.second);
    });
    return freud;
}

/**
 * @brief Allocator::unregisterSource : fetch the heap
 * @param source Sources::Source*
 * @return int
 */
heap_ptr_type Allocator::getHeap(heap_key_type uses)
{
    auto it = m_heaps.find(uses);
    if(it != m_heaps.end())
        return it->second;
    return nullptr;
}

registerUse_t registerUse(&Allocator::registerUse);
unregisterUse_t unregisterUse(&Allocator::unregisterUse);
unregisterAll_t unregisterAll(&Allocator::unregisterAll);
getHeap_t getHeap(&Allocator::getHeap);
getAllocatorStatus_t getAllocatorStatus(&Allocator::getStatus);

Buffers::Allocator allocator;

} // namespace Buffers

} // namespace drumlin
