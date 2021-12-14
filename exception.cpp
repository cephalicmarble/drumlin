#include "exception.h"

#include <sstream>
#include "drumlin.h"

namespace drumlin {

Exception::Exception() : exception()
{
    CPLATE;
    message = "Unknown Exception";
}

Exception::Exception(const Exception &rhs) : exception()
{
    CPLATE;
    message = rhs.message;
}

Exception::Exception(const string &str) : exception()
{
    CPLATE;
    message = str;
}

Exception::Exception(const std::stringstream &ss) : exception()
{
    CPLATE;
    message = ss.str();
}

Exception::~Exception()
{
    DPLATE;
}

ostream &operator <<(ostream &stream, const Exception &e)
{
    stream << e.message;
    return stream;
}

logger &operator <<(logger &stream, const Exception &e)
{
    stream << e.message;
    return stream;
}

} // namespace drumlin
