#include "logger.h"
#include "drumlin.h"

namespace drumlin {

bool debug = true;

std::recursive_mutex logger::s_critical_section;

logger::logger(ostream &strm):stream(strm)
{
    stream.flush();
    strm << boost::this_thread::get_id() << "::";
}

logger::logger(logger &rhs):stream(rhs.stream)
{
}

logger::~logger()
{
    stream << endl;
    stream.flush();
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

logger &logger::operator<<(const std::exception &e)
{
    stream << e.what();
    return *this;
}

ostream &logger::getStream()
{
    return stream;
}

}
