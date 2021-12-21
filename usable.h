#ifndef _USABLE_H
#define _USABLE_H

#include "drumlin.h"
#include "allocator.h"
#include "buffer.h"

namespace drumlin {

class UsesAllocator;

/**
 * @brief The SourceBuffer class : introduce new(void*)T and (T*)->~T()
 * against the pressure from the UsesAllocator's.
 */
template <typename T>
class UsableBuffer :
    public Buffers::HeapBuffer
{
    T m_instance;
public:
    UsesAllocator *source;
    UsableBuffer(byte* alloc, UseIdent const& use, string_list const& params)
    :Buffers::HeapBuffer(alloc, sizeof(*this), use),m_instance(params)
    {
        APLATE;
        assert(alloc == this);
    }
    virtual ~UsableBuffer()
    {
        BPLATE;
        &m_instance->~T();
    }
    T& getInstance() {
        return m_instance;
    }
    T *getInstancePtr() {
        return &m_instance;
    }
    operator T() {
        return m_instance;
    }
    T* operator ->() {
        return &m_instance;
    }
    void* operator new(size_t sz, size_t align, Buffers::heap_ptr_type heap)
    {
        return heap->alloc();
    }
};

template <typename T>
std::shared_ptr<UsableBuffer<T>>
make_usable_buffer(UsesAllocator* uses, UseIdent const& ident, string_list const& params)
{
    std::shared_ptr<UsableBuffer<T>> buffer;
    CPS_call([&buffer](Buffers::getHeap_t::Return & heap){
        if(!heap)return;
        UsableBuffer<T> *ptr(new (heap) UsableBuffer<T>);
        buffer.reset(ptr);
    }, Buffers::getHeap, uses);
    return buffer;
}

} // namespace drumlin

#endif // _USABLE_H
