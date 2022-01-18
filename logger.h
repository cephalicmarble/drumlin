#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>
#include <mutex>
using namespace std;
#include <boost/thread.hpp>
#include <boost/any.hpp>
using namespace boost;

namespace drumlin {

extern bool debug;

class Thread;

class logger
{
public:
    logger(ostream &strm);
    logger(logger &rhs);
    ~logger();
    logger &operator<<(const boost::thread::id &id);
    logger &operator<<(const string & str);
    logger &operator<<(const char *str);
    logger &operator<<(const unsigned long int i);
    logger &operator<<(const long int i);
    logger &operator<<(const unsigned int i);
    logger &operator<<(const int i);
    logger &operator<<(const unsigned short int i);
    logger &operator<<(const short int i);
    logger &operator<<(const unsigned char i);
    logger &operator<<(const char i);
    logger &operator<<(void* ptr);
    logger &operator<<(const std::exception &e);
    ostream &getStream();
    logger& operator<<(const boost::any& p);
    logger &operator<<(const Thread &e);
    static std::mutex s_critical_section;
private:
    ostream &stream;
};

} // namespace drumlin

#endif // LOGGER_H
