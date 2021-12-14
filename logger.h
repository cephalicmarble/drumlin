#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <iostream>
using namespace std;
#include <boost/thread.hpp>
using namespace boost;

namespace drumlin {

class logger
{
public:
    logger(ostream &strm);
    logger(logger &rhs);
    ~logger();
    operator ostream&();
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
    ostream &getStream();
private:
    ostream &stream;
};

} // namespace drumlin

#endif // LOGGER_H
