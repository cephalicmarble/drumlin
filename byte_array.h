#ifndef BYTE_ARRAY_H
#define BYTE_ARRAY_H

#include <string>
#include <iostream>
#include <fstream>
#include <istream>
#include <ostream>
#include <memory>
using namespace std;
#include <boost/array.hpp>
using namespace boost;
#include "object.h"
#include "gtypes.h"
#include "logger.h"
#include "string_list.h"

namespace drumlin {

class byte_array
{
    byte_array();
    byte_array(const void *mem,size_t length);
    byte_array(const string &owner);
public:
    typedef gint64 size_type;
    byte_array(byte_array &rhs);
    byte_array(byte_array &&rhs);
    ~byte_array();
    void clear();
    bool empty();
    byte_array operator=(const char *pc);
    byte_array operator=(const byte_array &pc);
    byte_array operator=(const byte_array &&pc);
    byte_array operator+=(const byte_array &pc);
    byte_array operator+(const byte_array &pc)const;
    static byte_array fromData(const char *pc,size_t start,size_t length);
    static byte_array fromData(void *mem,size_t length);
    static byte_array fromData(string str);
    static byte_array readAll(istream &strm);
    char *cdata()const{ return (char*)m_data; }
    void *data()const{ return m_data; }
    size_type  length()const{ return m_length; }
    void truncate(size_type len);
    operator std::string()const;
    operator string_list()const;
    void *release(){ m_destroy = false; return data(); }
    byte_array& append(std::string const& str);
    byte_array& append(const void *m_next,size_t length);
private:
    void *m_data;
    size_type m_length;
    bool m_destroy = false;
    friend ostream& operator<< (ostream& strm, const drumlin::byte_array &bytes);
    friend istream& operator>> (istream& strm, drumlin::byte_array &bytes);
};

/**
 * @brief buffers_type : the type of the socket's internal buffer vector
 */
typedef std::list<std::unique_ptr<byte_array>> buffers_type;

extern logger& operator<< (logger& strm, const drumlin::byte_array &bytes);
extern istream& operator>> (istream& strm, drumlin::byte_array &bytes);

} // namespace drumlin

#endif // BYTE_ARRAY_H
