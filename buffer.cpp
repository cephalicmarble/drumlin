#include "buffer.h"

namespace drumlin {

namespace Buffers {

/**
 * @brief Buffer::Buffer : only constructor, sets timestamp
 */
HeapBuffer::HeapBuffer(void* alloc, size_t _sizeof, UseIdent const& use)
 : m_data(alloc), m_len(_sizeof), m_ident(use), m_use_count(1),
 m_timestamp(posix_time::microsec_clock::universal_time())
{
}

HeapBuffer::~HeapBuffer()
{

}

/**
 * @brief Buffer::length
 * @return quint32
 */
guint32 HeapBuffer::length()const
{
    return m_len;
}

/**
 * @brief Buffer::operator byte_array
 */
HeapBuffer::operator byte_array()const
{
    return byte_array::fromRawData(m_data,m_len);
}

/**
 * @brief Buffer::operator string
 */
HeapBuffer::operator std::string()const
{
    return std::string(data<char>(),(int)length());
}

/**
 * @brief make_sub : construct a subs_map_type::value_type
 * @param rel Relevance
 * @param a Acceptor*
 * @return std::pair<const Relevance,Buffers::Acceptor*>
 */
std::pair<UseIdent,Buffers::Acceptor*> make_sub(UseIdent rel,Acceptor *a)
{
    return std::make_pair(rel,a);
}

} // namespace Buffers

} // namespace drumlin
