#ifndef _TERMINATOR_H
#define _TERMINATOR_H

#include <boost/thread.hpp>
using namespace boost;
#include "../drumlin/event.h"
#include "../drumlin/thread_worker.h"

namespace drumlin {

extern Application *iapp;

class Terminator
    : public ThreadWorker
{
    bool m_terminated = false;
public:
    Terminator();
    ~Terminator();
    virtual void work(Object *,std::shared_ptr<Event>);
    virtual bool event(std::shared_ptr<Event>);
    virtual void shutdown();
    void run();
};

} // namespace drumlin

#endif // _TERMINATOR_H
