#ifndef __USES_ALLOCATOR_H
#define __USES_ALLOCATOR_H

#include <memory>
#include <utility>
#include "tao_forward.h"
using namespace tao;
#include "drumlin.h"
#include "gtypes.h"
#include "allocator.h"
#include "work.h"
#include "buffer.h"
#include "usable.h"

namespace drumlin {

class ThreadWorker;
class UseIdent;

namespace Buffers {

}

/**
 * @brief The UsesAllocator class : represents a heap user.
 * In fact two uses (past & future), and allows for
 * migration from one to the other.
 */
class UsesAllocator
    : public std::enable_shared_from_this<UsesAllocator>
{
    typedef std::enable_shared_from_this<UsesAllocator> base;

    std::weak_ptr<ThreadWorker> m_worker;

    guint32 m_data_length;                  // sizeof
    guint32 m_buffer_size;                  // backlog
    time_t m_tau;                       // tick-duration
    time_t m_process;                   // process-duration

    std::string m_token;
    guint64 m_memory_allocated;
public:
    UsesAllocator(std::string _token);
    UsesAllocator(UsesAllocator & rhs);
    UsesAllocator(UsesAllocator && rvalue);
    UsesAllocator& operator=(UsesAllocator & rhs);
    UsesAllocator& operator=(UsesAllocator && rhs);
    virtual ~UsesAllocator();

    string const& getToken()const { return m_token; }
    UsesAllocator& setDataLength(guint32 data_length) { m_data_length = data_length; return *this; }
    UsesAllocator& setBackLength(guint32 ttl) { m_buffer_size = ttl; return *this; }
    UsesAllocator& setTau(time_t tau) { m_tau = tau; return *this; }
    UsesAllocator& setProcessLength(time_t length) { m_process = length; return *this; }
    guint32 getDataLength() { return m_data_length; }
    guint32 getBackLength() { return m_buffer_size; }
    time_t getTau() { return m_tau; }
    time_t getProcessLength() { return m_process; }

    UsesAllocator withTauProcess(time_t tau, time_t process);

    guint64 getQueueSize() {
        return std::ceil(m_buffer_size * (m_tau / m_process));
    }

    template <typename T>
    std::shared_ptr<UsableBuffer<T>> alloc(Buffers::heap_key_type uses)
    {
        auto heap(Buffers::allocator.getHeap(uses));
        return new(heap) UsableBuffer<T>;
    }
    template <typename T>
    int free(UsableBuffer<T> *buffer)
    {
        auto heap(getHeap(buffer->getUseIdent()->getUse()));
        buffer->~HeapBuffer();
        int ret = buffer->length();
        heap->free((byte*)buffer);
        return ret;
    }

    void getStatus(json::value *status);

    void report(json::value *obj,ReportType type);
};

} // namespace drumlin

#endif // __USES_ALLOCATOR_H
