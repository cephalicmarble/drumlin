#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>
#include <exception>
#include <sstream>
using namespace std;

#include "logger.h"

namespace drumlin {

/**
 * @brief The Exception class : inherited Exception
 */
class Exception : public std::exception
{
public:
    string message;
    Exception();
    Exception(const Exception &rhs);
    Exception(const string &str);
    Exception(const std::stringstream &ss);
    ~Exception();
    const char*what(){return message.c_str();}
    void raise() const { throw *this; }
    Exception *clone() const { return new Exception(*this); }

    friend ostream &operator <<(ostream &stream, const Exception &e);
    friend logger &operator <<(logger &stream, const Exception &e);
};

} // namespace drumlin

#endif // EXCEPTION_H
