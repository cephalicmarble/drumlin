#include "logger.h"

namespace drumlin {

logger::logger(ostream &strm):stream(strm)
{

}

logger::logger(logger &rhs):stream(rhs.stream)
{

}

logger::~logger()
{
    stream << endl;
}

logger::operator ostream&()
{
    stream << " ";
    return stream;
}

logger &logger::operator<<(const boost::thread::id &id)
{
    stream << " " << id;
    return *this;
}

logger &logger::operator<<(const string & str)
{
    stream << " " << str;
    return *this;
}

logger &logger::operator<<(const char *str)
{
    stream << " " << str;
    return *this;
}

logger &logger::operator<<(const unsigned long int i)
{
    stream << " " << i;
    return *this;
}

logger &logger::operator<<(const long int i)
{
    stream << " " << i;
    return *this;
}

logger &logger::operator<<(const unsigned int i)
{
    stream << " " << i;
    return *this;
}

logger &logger::operator<<(const int i)
{
    stream << " " << i;
    return *this;
}

logger &logger::operator<<(const unsigned short int i)
{
    stream << " " << i;
    return *this;
}

logger &logger::operator<<(const short int i)
{
    stream << " " << i;
    return *this;
}

logger &logger::operator<<(const unsigned char i)
{
    stream << " " << i;
    return *this;
}

logger &logger::operator<<(const char i)
{
    stream << " " << i;
    return *this;
}

logger &logger::operator<<(void* ptr)
{
    stream << " " << ptr;
    return *this;
}

ostream &logger::getStream()
{
    return stream;
}

}
