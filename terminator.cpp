#include "terminator.h"

void Terminator::run()
{
    if(iapp){
        iapp->stop();
        if(!restarting){
            make_event(DrumlinEventApplicationShutdown,"Terminator::shutdown",(Object*)0)->punt();
        }else{
            make_event(DrumlinEventApplicationRestart,"Terminator::restart",(Object*)0)->punt();
        }
    }
}
