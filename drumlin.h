#ifndef DRUMLIN_H
#define DRUMLIN_H

#include <iostream>
#include <fstream>
#include <cstdio>
#include <memory>
using namespace std;
#include <boost/any.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/for_each.hpp>
#include <boost/filesystem/path.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <chrono>
using namespace boost;
#include "logger.h"
using namespace drumlin;

namespace drumlin {

typedef std::chrono::microseconds time_duration_t;

}

#define Debug() if(drumlin::debug) drumlin::logger(std::cerr)
#define Critical() drumlin::logger(std::cerr) << "********"

// for ThreadAccessor
#define FRIENDTHREADSLOCK std::lock_guard<std::recursive_mutex> l(const_cast<std::recursive_mutex&>(iapp->m_critical_section));
// for internal Application use
#define THREADSLOCK std::lock_guard<std::recursive_mutex> l(const_cast<std::recursive_mutex&>(m_critical_section));

#define PROMISELOCK std::lock_guard<std::recursive_mutex> l(const_cast<std::recursive_mutex&>(m_critical_section));

// for internal brief use
#define INTERNAL std::lock_guard<std::recursive_mutex> internal(const_cast<std::recursive_mutex&>(m_mutex));
// for external brief use
#define INTERNALOP(thread) std::lock_guard<std::recursive_mutex> l(const_cast<std::recursive_mutex&>(thread.m_mutex));
// for the loops
#define CRITICAL std::lock_guard<std::recursive_mutex> critical(const_cast<std::recursive_mutex&>(m_critical_section));
// external version
#define CRITICALOP(thread) std::lock_guard<std::recursive_mutex> l(const_cast<std::recursive_mutex&>(thread.m_critical_section));

// #define INTERNAL {;}
// #define INTERNALOP(a) {;}
// #define CRITICAL {;}
// #define CRITICALOP(a) {;}

#define THREADLOG2(a, b) {LOGLOCK;Debug() << __func__ << a << b << *this;}

#define EVENTLOG(pevent) {LOGLOCK;Debug() \
                            << __func__ \
                            << metaEnum<DrumlinEventType>().toString((DrumlinEventType)pevent->type()) \
                            << pevent->getName();}
#define EVENTLOG1(pevent, a) {LOGLOCK;Debug() \
                            << __func__ \
                            << metaEnum<DrumlinEventType>().toString((DrumlinEventType)pevent->type()) \
                            << pevent->getName() \
                            << a ;}

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

class temp_file;
typedef std::shared_ptr<temp_file> temp_file_ptr;
class temp_file
{
    boost::filesystem::path m_path;
    int m_fd;
public:
    temp_file() {
        std::string name = "tempXXXXXX";
        m_fd = mkstemp(const_cast<char*>(name.c_str()));
        m_path = boost::filesystem::path(name);
    }
    temp_file(const boost::filesystem::path &root): m_path(root) {
        m_fd = open(get_path().c_str(), O_CREAT | O_TMPFILE | O_RDWR);
    }
    ~temp_file() {
        close(m_fd);
        remove(m_path.c_str());
    }
    const boost::filesystem::path &get_path() const { return m_path; }
    std::ofstream get_ofstream() const {
        return std::ofstream(get_path().c_str());
    }
    std::ifstream get_ifstream() const {
        return std::ifstream(get_path().c_str());
    }
};

} // namespace drumlin

#endif // DRUMLIN_H
