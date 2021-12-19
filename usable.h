#ifndef _USABLE_H
#define _USABLE_H

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
        assert(alloc == this);
    }
    virtual ~UsableBuffer()
    {
        &m_instance->~T();
    }
    template <>
    T& getInstance() {
        return m_instance;
    }
    template <>
    T *getInstancePtr() {
        return &m_instance;
    }
    template <>
    operator T() {
        return m_instance;
    }
    template <>
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
make_usable_buffer(std::weak_ptr<UsesAllocator> const& uses, UseIdent const& ident, string_list const& params)
{
    std::shared_ptr<UsableBuffer<T>> buffer(
             new (drumlin::Buffers::allocator.getHeap(ident.getUse())) UsableBuffer<T>
        );
    return buffer;
}

} // namespace drumlin

#endif // _USABLE_H
