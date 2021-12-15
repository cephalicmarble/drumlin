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
    std::string event(pevent->getName());
    switch (pevent->type())
    {
        case DrumlinEventThreadNotify:
            if (event == "test-success" || event == "test-failure")
            {
                {LOGLOCK;Debug() << "FROM GTEST:" << pevent->getName();}
                event::punt(event::make_event(DrumlinEventApplicationClose,__func__));
            } else if (event == "beforeWork" || event == "beforeStart") {
                {LOGLOCK;Debug() << "Terminator:" << event;}
            }
            return true;
        default:
            return false;
    }
}

void Terminator::run()
{
    //static char pr[] = {'c','#'};
    static char pr[] = {'s', 't'};
    static char *pc = pr;
    std::string str;
    if (std::distance(pr, pc++) > 2) {
        std::cin >> str;
    } else {
        str = *pc;
    }
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
            event::punt(event::make_event(DrumlinEventApplicationClose,__func__));
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
