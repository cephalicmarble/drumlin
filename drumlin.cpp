#include "drumlin.h"

#include <boost/mpl/for_each.hpp>

namespace drumlin {

logger& operator<< (logger& strm, const boost::any& p)
{
    mpl::for_each<value_types>(stream_operator_impl(strm,p));
    return strm;
}

}
