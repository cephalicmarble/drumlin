#ifndef _BUFFER_H
#define _BUFFER_H

#include <string>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/chrono.hpp>
#include "gtypes.h"
#include "byte_array.h"
#include "use_ident.h"
#include "allocator.h"
#include "exception.h"

namespace drumlin {

class UsesAllocator;

namespace Buffers {

/**
 * @brief The Buffer class : represents a chunk of sample
 */
class HeapBuffer
{
public:
    typedef void* ptr_type;
protected:
    ptr_type m_data;
    guint32 m_len;
    UseIdent m_ident;
    guint32 m_use_count;
    boost::posix_time::ptime m_timestamp;
    HeapBuffer(void* alloc, size_t _sizeof, UseIdent const& use);
public:
    virtual ~HeapBuffer();
    bool isValid()const{ return !!m_data; }

    UseIdent const& getUseIdent()const{ return m_ident; }
    boost::posix_time::ptime const& getTimestamp()const{ return m_timestamp; }

    virtual void flush(){}
    guint32 release() { return --m_use_count; }
    /**
     * template<class T> const T*data();
     */
    template <class T>
    const T *data()const{ return (const T*)m_data; }
    guint32 length()const;

    virtual operator std::string()const;
    virtual operator byte_array()const;

    friend class Allocator;
};

/**
 * @brief The Acceptor class
 */
class Acceptor
{
public:
    virtual void accept(std::weak_ptr<Buffers::HeapBuffer> buffer)=0;
    virtual void flush(std::weak_ptr<Buffers::HeapBuffer> buffer)=0;
};

extern std::pair<UseIdent,Buffers::Acceptor*> make_sub(UseIdent rel,Acceptor *a);

} // namespace Buffers

} // namespace drumlin

#endif // _BUFFER_H
