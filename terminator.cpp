#include "terminator.h"

#include "drumlin.h"
#include "thread_worker.h"
#include "thread_accessor.h"

Terminator::Terminator()
: ThreadWorker(ThreadType_terminator)
{
    APLATE;
}

Terminator::~Terminator()
{
    BPLATE;
}

void Terminator::work(Object *,std::shared_ptr<Event> pevent)
{
    PLATE1(*pevent);
    if(!m_terminated)
        run();
}

bool Terminator::event(std::shared_ptr<Event> pevent)
{
    EVENTLOG(pevent);
    switch (pevent->type())
    {
        case DrumlinEventThreadNotify:
            std::string event(pevent->getName());
            if (event == "test-success" || event == "test-failure")
            {
                {LOGLOCK;Debug() << "GTEST:" << pevent->getName();}
            } else if (event == "beforeWork" || event == "beforeStart") {
                {LOGLOCK;Debug() << "Terminator:" << event;}
            }
            return true;
    }
    return false;
}

void Terminator::run()
{
    std::string str("c");
    //std::cin >> str;
    switch(*str.begin()) {
        case 't':
            ThreadAccessor()
                .named("test-worker")
                .getNamed()
                (SendEvent(event::make_event(DrumlinEventThreadWork, "run-tests")));
                break;
        case 's':
            ThreadAccessor()
                .named("test-worker")
                .getNamed()
                (StartThread());
                break;
        case 'c':
            event::punt(event::make_event(DrumlinEventApplicationClose,
                __func__,(Object*)(*str.begin() == 'r' || *str.begin() == 'R')));
            break;
        default:
            boost::this_thread::yield();
            boost::this_thread::sleep(boost::posix_time::milliseconds(400));
    }
}

void Terminator::shutdown()
{
    PLATE;
};
