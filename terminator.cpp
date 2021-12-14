#include "terminator.h"

void Terminator::run()
{
    if(iapp){
        iapp->stop();
        if(!restarting){
            event::punt(event::make_event(DrumlinEventApplicationShutdown,"Terminator::shutdown",(Object*)0));
        }else{
            event::punt(event::make_event(DrumlinEventApplicationRestart,"Terminator::restart",(Object*)0));
        }
    }
}
