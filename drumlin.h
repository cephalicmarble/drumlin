#ifndef DRUMLIN_H
#define DRUMLIN_H

#include <iostream>
#include <cstdio>
using namespace std;
#include <boost/any.hpp>
#include <boost/mpl/vector.hpp>
using namespace boost;
#include "logger.h"
using namespace drumlin;

#define Debug() if(drumlin::debug) drumlin::logger(std::cerr)
#define Critical() drumlin::logger(std::cerr) << "********"

#define CRITICAL std::lock_guard<std::recursive_mutex> l(const_cast<std::recursive_mutex&>(m_critical_section));
#define CRITICALOP(thread) std::lock_guard<std::recursive_mutex> l(const_cast<std::recursive_mutex&>(thread.m_critical_section));

#define THREADLOG2(a, b) {LOGLOCK;Debug() << __func__ << a << b << *this;}

#define EVENTLOG(pevent) {LOGLOCK;Debug() \
                            << __func__ \
                            << metaEnum<DrumlinEventType>().toString((DrumlinEventType)pevent->type()) \
                            << pevent->getName();}

/**
 * LOGLOCK holds the logger mutex, so use wrap it in a block and preferably
 * use a logging macro defined here or in your file.
 */
#define LOGLOCK std::lock_guard<std::mutex> l(logger::s_critical_section);

/**
 *  CPLATE & DPLATE require contexts that may be streamed to drumlin::logger.
 *  APLATE & BPLATE do not, and give only the this pointer value.
 *  PLATE & PLATE1 use __FILE__ to prompt this, this & a value and
 *  PLATE2 gives name instead of __FILE__ and a value.
 */
#define APLATE {LOGLOCK;Debug() << "new.A" << __func__ << ":" << this;}
#define CPLATE {LOGLOCK;Debug() << "new.C" << __func__ << *this << ":" << this;}
#define DPLATE {LOGLOCK;Debug() << "del.D" << __func__ << *this << ":" << this;}
#define BPLATE {LOGLOCK;Debug() << "del.B" << __func__ << ":" << this;}
#define  PLATE {LOGLOCK;Debug() << __FILE__ << __func__ << ":" << this;}
#define PLATE1(value) {LOGLOCK;Debug() << __FILE__ << __func__ << ":" << this << value;}
#define PLATE2(name,value) {LOGLOCK;Debug() << name << __func__ << ":" << this << value;}

namespace drumlin {

typedef mpl::vector<bool, short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long, float, double, long double, std::string> value_types;
struct stream_operator_impl {
    stream_operator_impl(ostream &_strm, const boost::any& _p):strm(_strm),p(_p){}
    template <typename Any>
    ostream& operator()(Any &){
        if(p.type()==typeid(Any)){
            strm << " " << any_cast<Any>(p);
        }
        return strm;
    }
private:
    ostream &strm;
    const boost::any& p;
};

} // namespace drumlin

#endif // DRUMLIN_H
