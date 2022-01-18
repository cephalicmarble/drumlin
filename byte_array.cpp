#include "byte_array.h"

#include <string>
#include <sstream>
#include <memory>
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

byte_array::byte_array(const void *mem,size_t length)
{
    APLATE;
    m_data = const_cast<void*>(mem);
    m_length = length;
    m_destroy = true;
}

byte_array::byte_array(const std::string &owner)
{
    APLATE;
    m_data = const_cast<void*>(static_cast<const void*>(owner.c_str()));
    m_length = owner.length() + 1;
    m_destroy = false;
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

bool byte_array::empty()
{
    return m_data == nullptr && m_length == 0;
}

byte_array byte_array::operator=(const byte_array &rhs)
{
    clear();
    append(rhs.m_data,rhs.m_length);
    return *this;
}

byte_array byte_array::operator=(const byte_array &&rhs)
{
    clear();
    m_destroy = rhs.m_destroy;
    m_data = rhs.m_data;
    m_length = rhs.m_length;
    return *this;
}

byte_array byte_array::operator=(const char *pc)
{
    clear();
    m_data = const_cast<void*>(static_cast<const void*>(pc));
    m_length = strlen(pc) + 1;
    return *this;
}

byte_array byte_array::operator+=(const byte_array &pc)
{
    append(pc.cdata(), pc.length());
    return *this;
}

byte_array byte_array::operator+(const byte_array &pc)const
{
    byte_array tmp(*this);
    tmp.append(pc.cdata(), pc.length());
    return tmp;
}

byte_array byte_array::fromData(const char *pc,size_t start,size_t length)
{
    byte_array bytes;
    bytes.append(pc+start,length + 1);
    return bytes;
}

byte_array byte_array::fromData(void *mem,size_t length)
{
    byte_array bytes;
    bytes.append(mem,length);
    return bytes;
}

byte_array byte_array::fromData(std::string str)
{
    byte_array bytes;
    bytes.append(str.c_str(),str.length() + 1);
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

void byte_array::truncate(size_type len)
{
    if(m_data && m_length >= len){
        if(!m_destroy) {
            append(m_data, len);
        }
        ((char*)m_data)[(m_length=len)] = 0;
    }
}

byte_array::operator std::string()const
{
    return m_length?std::string((char*)m_data,0,m_length):std::string();
}

byte_array::operator string_list()const
{
    string_list list;
    char *pc((char*)m_data);
    while(pc < ((char*)m_data) + m_length) {
        std::string s(pc);
        list << s;
        pc += s.length() + 1;
    }
    return list;
}

byte_array& byte_array::append(std::string const& str)
{
    append(str.c_str(),str.length() + 1);
    return *this;
}

byte_array& byte_array::append(const void *m_next,size_t length)
{
    if(!length)
        return *this;
    char *pdest;
    if(m_destroy && m_data){
        pdest = (char*)realloc(m_data,m_length+length);
    }else if(!m_data){
        pdest = (char*)malloc(m_length+length);
    }
    memmove(pdest+m_length,m_next,length);
    m_data = pdest;
    m_length += length;
    m_destroy = true;
    return *this;
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
