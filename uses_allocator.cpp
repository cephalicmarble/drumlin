#define TAOJSON
#include "uses_allocator.h"

#include <string>
#include <memory>
#include <utility>
#include "drumlin.h"
#include "use_ident.h"
#include "work.h"

namespace drumlin {

UsesAllocator::UsesAllocator(std::string _token)
: base(), m_token(_token)
{
    APLATE;
}

UsesAllocator::UsesAllocator(UsesAllocator & rhs)
: base()
{
    APLATE;
    *this = rhs;
}

UsesAllocator::UsesAllocator(UsesAllocator && rvalue)
: base()
{
    PLATE1("rvalue");
    *this = rvalue;
}

UsesAllocator& UsesAllocator::operator=(UsesAllocator & rhs)
{
    m_token         = rhs.m_token;              // ident root
    m_data_length   = rhs.m_data_length;        // sizeof
    m_buffer_size   = rhs.m_buffer_size;        // backlog
    m_tau           = rhs.m_tau;                // tick-duration
    m_process       = rhs.m_process;            // process-duration
    m_memory_allocated = rhs.m_memory_allocated;// cumulative
    return *this;
}

UsesAllocator& UsesAllocator::operator=(UsesAllocator && rhs)
{
    m_token         = std::move(rhs.m_token);
    m_data_length   = rhs.m_data_length;
    m_buffer_size   = rhs.m_buffer_size;
    m_tau           = rhs.m_tau;
    m_process       = rhs.m_process;
    m_memory_allocated = rhs.m_memory_allocated;
    return *this;
}

/**
 * @brief UsesAllocator::~UsesAllocator : flush our buffers
 */
UsesAllocator::~UsesAllocator()
{
    // const UseIdent rel(std::move(UseIdent::fromUsesAllocator(this)));
    // Cache(CPS_call_void(Buffers::clearRelevantBuffers,&rel));
}

UsesAllocator UsesAllocator::withTauProcess(time_duration_t tau, time_duration_t process)
{
    return UsesAllocator(*this).setTau(tau).setProcessLength(process);
}

// /**
//  * @brief UsesAllocator::writeNext : prepares a new buffer read from this UsesAllocator
//  */
// void UsesAllocator::writeNext()
// {
//     Buffers::UsableBuffer *buf(new Buffers::UsableBuffer(this));
//     if(!buf->isValid()){
//         delete buf;
//         return;
//     }
//     writeNextHere(const_cast<void*>(buf->data<void>()),buf->length());
//     buf->getUseIdent()->setUsesAllocator(this);
//     buf->TTL(getTTL());
//     Cache(CPS_call_void(Buffers::addUsableBuffer,const_cast<const Buffers::UsableBuffer*>(buf)));
// }
//
// void UsesAllocator::writeToFile(Request *req,string rpath)
// {
//     Files::files.add(req->getUseIdentRef(),rpath);
// }

void UsesAllocator::getStatus(json::value *status)
{
    status->get_object().insert({"actions","chart,record"});
}

void UsesAllocator::report(json::value *obj,ReportType type)
{
    obj->get_object().insert({std::string("process"),m_process.count()});
    if(type & WorkObject::ReportType::Memory){
        CPS_call([&obj](Buffers::heap_ptr_type heap)->void{
            if(!heap)
                return;
            obj->get_object().insert({"allocated",heap->allocated * heap->size});
            obj->get_object().insert({"reserved",heap->max});
        }, Buffers::getHeap, this);
    }
}

} // namespace drumlin
