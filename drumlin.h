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

#define Debug() drumlin::logger(std::cerr)
#define Critical() drumlin::logger(std::cerr) << "********"

typedef mpl::vector<bool, short, unsigned short, int, unsigned int, long, unsigned long, long long, unsigned long long, float, double, long double, std::string> value_types;
struct stream_operator_impl {
    stream_operator_impl(ostream &_strm, const boost::any& _p):strm(_strm),p(_p){}
    template <typename Any>
    ostream& operator()(Any &){
        if(p.type()==typeid(Any)){
            strm << any_cast<Any>(p);
        }
        return strm;
    }
private:
    ostream &strm;
    const boost::any& p;
};
extern logger& operator<< (logger& strm, const boost::any& p);

#endif // DRUMLIN_H
