#include "terminator.h"

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>

#include "../drumlin/drumlin.h"
#include "../drumlin/thread_worker.h"
#include "../drumlin/thread_accessor.h"

#include "behaviour.h"

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
        case DrumlinEventThreadWork:
            if (event == "tick") {
                return true;
            }
            return false;
        default:
            return false;
    }
}

void Terminator::run()
{
    // command.first,.second,.third
    static string input = "[test-worker]:[]";
    //static char pr[] = {'c','#'};
    static char pr[] = {'s', 't', 'g', 'c'};
    static char *pc = pr;
    std::string str;
    if (std::distance(pr, pc++) > (int)sizeof(pr) - 1) {

        //std::cin >> str;
        char *line(readline ("[readline] "));
        std::string str(line);

        //tao::pegtl::parse< grammar::addition, grammar::my_action >( str );
    } else {
        str = *pc;
    }
    // typedef std::pair<std::function<std::string()>, std::string> keyword;
    // (str) {
    //
    // }
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
