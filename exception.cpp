#include "exception.h"

#include <sstream>
#include "drumlin.h"

namespace drumlin {

Exception::Exception() : exception()
{
    message = "Unknown Exception";
}

Exception::Exception(const Exception &rhs) : exception()
{
    message = rhs.message;
}

Exception::Exception(const string &str) : exception()
{
    message = str;
}

Exception::Exception(const std::stringstream &ss) : exception()
{
    message = ss.str();
}

} // namespace drumlin
