#include "byte_array.h"

#include <string>
#include <sstream>
using namespace std;
#include "drumlin.h"

namespace drumlin {

byte_array::byte_array()
{
    APLATE;
    m_destroy = false;
    m_data = nullptr;
    m_length = 0;
}

byte_array::byte_array(byte_array &rhs)
{
    APLATE;
    operator=(rhs);
}

byte_array::byte_array(byte_array &&rhs)
{
    APLATE;
    operator=(rhs);
}

byte_array::byte_array(const void *mem,size_t length,bool takeOwnership)
{
    APLATE;
    m_data = const_cast<void*>(mem);
    m_length = length;
    m_destroy = takeOwnership;
}

byte_array::byte_array(const std::string &owner)
{
    APLATE;
    m_data = const_cast<void*>(static_cast<const void*>(owner.c_str()));
    m_length = owner.length();
}

byte_array::~byte_array()
{
    BPLATE;
    clear();
}

void byte_array::clear()
{
    if(m_destroy && m_data != nullptr){
        free(m_data);
        m_destroy = false;
    }
    m_data = nullptr;
    m_length = 0;
}

byte_array &byte_array::operator=(const byte_array &rhs)
{
    clear();
    m_destroy = rhs.m_destroy;
    const_cast<byte_array&>(rhs).m_destroy = false;
    m_data = rhs.m_data;
    m_length = rhs.m_length;
    return *this;
}

byte_array &byte_array::operator=(const char *pc)
{
    clear();
    m_data = const_cast<void*>(static_cast<const void*>(pc));
    m_length = strlen(pc);
    return *this;
}

byte_array byte_array::fromRawData(char *pc,size_t start,size_t length)
{
    byte_array bytes;
    bytes.append(pc+start,length!=string::npos?length:strlen(pc+start));
    return bytes;
}

byte_array byte_array::fromRawData(void *mem,size_t length)
{
    byte_array bytes;
    bytes.append(mem,length);
    return bytes;
}

byte_array byte_array::fromRawData(std::string str)
{
    byte_array bytes;
    bytes.append(str.c_str(),str.length());
    return bytes;
}

byte_array byte_array::readAll(istream &strm)
{
    stringstream ss;
    char pc[1024];
    streamsize sz = sizeof(pc);
    while(sizeof(pc) == (sz = strm.readsome(pc,sizeof(pc)))){
        ss << std::string(pc,sz);
    }
    byte_array bytes;
    bytes.append(ss.str().c_str(),ss.str().length());
    return bytes;
}

void byte_array::append(std::string const& str)
{
    append(str.c_str(),str.length());
}

void byte_array::append(std::string & str)
{
    append(str.c_str(),str.length());
}

void byte_array::append(const void *m_next,size_t length)
{
    if(!length)
        return;
    char *pdest;
    if(m_data){
        pdest = (char*)realloc(m_data,m_length+length);
    }else{
        pdest = (char*)malloc(m_length+length);
    }
    memmove(pdest+m_length,m_next,length);
    if(m_data != nullptr)free(m_data);
    m_data = pdest;
    m_length += length;
    m_destroy = true;
}

/**
     * @brief Buffer::Buffer : copy from data
     * @param _data void*
     * @param _len qint64
     */
ByteBuffer::ByteBuffer(void*_data,gint64 _len):type(FreeBuffer)
{
    APLATE;
    buffers.free_buffer.len = _len;
    buffers.free_buffer.data = (char*)malloc(buffers.free_buffer.len);
    memcpy(buffers.free_buffer.data,_data,buffers.free_buffer.len);
}

/**
     * @brief Buffer::Buffer : copy from byte_array
     * @param bytes byte_array
     */
ByteBuffer::ByteBuffer(byte_array const& bytes, bool freeAfterUse):type(freeAfterUse ? FreeBuffer : TempBuffer)
{
    APLATE;
    buffers.free_buffer.len = bytes.length();
    if (freeAfterUse) {
        buffers.free_buffer.data = (char*)malloc(buffers.free_buffer.len);
        memcpy(buffers.free_buffer.data, bytes.data(), buffers.free_buffer.len);
    } else {
        buffers.free_buffer.data = (char*)bytes.data();
    }
}

/**
     * @brief Buffer::Buffer : copy from tring
     * @param  tring
     */
ByteBuffer::ByteBuffer(string const& _str):type(FreeBuffer)
{
    APLATE;
    const char *str(_str.c_str());
    buffers.free_buffer.len = strlen(str);
    buffers.free_buffer.data = strdup(str);
}

/**
     * @brief Buffer::Buffer : copy from cstr
     * @param cstr const char*
     */
ByteBuffer::ByteBuffer(const char *cstr):type(FreeBuffer)
{
    APLATE;
    buffers.free_buffer.len = strlen(cstr);
    buffers.free_buffer.data = strdup(cstr);
}

/**
     * @brief Buffer::Buffer : reference a buffer
     * @param buffer Buffers::Buffer*
     */
ByteBuffer::ByteBuffer(const IBuffer *buffer):type(CacheBuffer)
{
    APLATE;
    buffers._buffer = buffer;
}

/**
     * @brief Buffer::~Buffer : free any copied data
     */
ByteBuffer::~ByteBuffer()
{
    BPLATE;
    if(type == FreeBuffer){
        free(buffers.free_buffer.data);
    }
}
/**
     * @brief Buffer::length
     * @return qint64
     */
gint64 ByteBuffer::length()
{
    switch(type){
    case TempBuffer:
    case FreeBuffer:
        return buffers.free_buffer.len;
    case CacheBuffer:
        return buffers._buffer->length();
    }
    return 0;
}
/**
     * @brief Buffer::operator byte_array
     */
ByteBuffer::operator byte_array()
{
    switch(type){
    case TempBuffer:
    case FreeBuffer:
        return byte_array::fromRawData(const_cast<void*>(data<void>()),length());
    case CacheBuffer:
        return byte_array::fromRawData(const_cast<void*>(buffers._buffer->data()),buffers._buffer->length());
    default:
        return byte_array();
    }
}

ostream& operator<< (ostream &strm, const drumlin::byte_array &bytes)
{
    strm.write((char*)bytes.data(),bytes.length());
    return strm;
}

istream& operator>> (istream& strm, drumlin::byte_array &bytes)
{
    stringstream ss;
    ss << strm.rdbuf();
    bytes.append(ss.str().c_str(),ss.str().length());
    return strm;
}

} //namespace drumlin
