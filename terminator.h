#ifndef _TERMINATOR_H
#define _TERMINATOR_H

#include <boost/thread.hpp>
using namespace boost;
#include "event.h"

namespace drumlin {

extern ApplicationBase *iapp;

class Terminator
{
    bool restarting = false;
public:
    Terminator(bool _restarting = false):restarting(_restarting),m_thread(&Terminator::run,this){}
    void run();
private:
    boost::thread m_thread;
};

} // namespace drumlin

#endif // _TERMINATOR_H
