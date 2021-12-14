#include "terminator.h"

#include "drumlin.h"
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
    run();
}

bool Terminator::event(std::shared_ptr<Event> pevent)
{
    PLATE1(*pevent);
    run();
    return true;
}

void Terminator::run()
{
    PLATE;
    while(true) {
        std::string str;
        std::cin >> str;
        if (str == " ") {
            ThreadAccessor access;
            access.named("test-worker");
            access.getNamed();
            access(SendEvent(event::make_event(DrumlinEventThreadWork, "grindstone")));
        }else{
            event::punt(event::make_event(DrumlinEventApplicationClose,__func__,(Object*)(*str.begin() == 'r' || *str.begin() == 'R')));
            break;
        }
    }
}
