#define TAOJSON
#include "signalhandler.h"

#include <assert.h>
#include <boost/lexical_cast.hpp>
using namespace boost;
#include "drumlin.h"
#include "cursor.h"
#include "event.h"
#include "application.h"

#ifdef _WIN32
# include <windows.h>
# include <set>
#else
# include <signal.h>
#endif //!_WIN32

namespace drumlin {

// There can be only ONE SignalHandler per process
SignalHandler* g_handler(nullptr);

#ifdef _WIN32

BOOL WINAPI WIN32_handleFunc(DWORD);
gremlin::SignalType WIN32_physicalToLogical(DWORD);
DWORD WIN32_logicalToPhysical(gremlin::SignalType);
std::set<gremlin::SignalType> g_registry;

#else //_WIN32

void POSIX_handleFunc(int);
gremlin::SignalType POSIX_physicalToLogical(int);
int POSIX_logicalToPhysical(gremlin::SignalType);

#endif //_WIN32

SignalHandler::SignalHandler(int mask) : _mask(mask)
{
    g_handler = this;

#ifdef _WIN32
    SetConsoleCtrlHandler(WIN32_handleFunc, TRUE);
#endif //_WIN32

    for (guint8 i=0;i<numSignals;i++)
    {
        guint8 logical = 0x1 << i;
        if (_mask & logical)
        {
#ifdef _WIN32
            g_registry.insert(logical);
#else
            int sig = POSIX_logicalToPhysical((gremlin::SignalType)logical);
            bool failed = signal(sig, POSIX_handleFunc) == SIG_ERR;
            assert(!failed);
            (void)failed; // Silence the warning in non _DEBUG; TODO: something better

#endif //_WIN32
        }
    }

}

SignalHandler::~SignalHandler()
{
    Debug() << "delete SignalHandler";
#ifdef _WIN32
    SetConsoleCtrlHandler(WIN32_handleFunc, FALSE);
#else
    for (guint8 i=0;i<numSignals;i++)
    {
        guint8 logical = 0x1 << i;
        if (_mask & logical)
        {
            signal(POSIX_logicalToPhysical((gremlin::SignalType)logical), SIG_DFL);
        }
    }
#endif //_WIN32
}

bool SignalHandler::handleSignal(gremlin::SignalType signal)
{
    if(Tracer::tracer!=nullptr){
        Tracer::endTrace();
    }
    event::punt(event::make_event(DrumlinEventApplicationShutdown,
        gremlin::metaEnum<SignalType>().toString(signal),(Object*)(gint64)signal));
    return true;
}

#ifdef _WIN32
DWORD WIN32_logicalToPhysical(gremlin::SignalType signal)
{
    switch ((SignalType)signal)
    {
    case SignalType_int:    return CTRL_C_EVENT;
    case SignalType_term:   return CTRL_BREAK_EVENT;
    case SignalType_close:  return CTRL_CLOSE_EVENT;
    default:
        return ~(unsigned int)0; // SIG_ERR = -1
    }
}
#else
int POSIX_logicalToPhysical(gremlin::SignalType signal)
{
    switch (signal)
    {
    case SignalType_int:    return SIGINT;
    case SignalType_term:   return SIGTERM;
    // In case the client asks for a SIG_CLOSE handler, accept and
    // bind it to a SIGTERM. Anyway the signal will never be raised
    case SignalType_close:  return SIGTERM;
    case SignalType_reload: return SIGHUP;
    case SignalType_segv:   return SIGSEGV;
    default:
        return -1; // SIG_ERR = -1
    }
}
#endif //_WIN32


#ifdef _WIN32
gremlin::SignalType WIN32_physicalToLogical(DWORD signal)
{
    switch (signal)
    {
    case CTRL_C_EVENT:      return SignalType_int;
    case CTRL_BREAK_EVENT:  return SignalType_term;
    case CTRL_CLOSE_EVENT:  return SignalType_close;
    default:
        return SignalType_unhandled;
    }
}
#else
gremlin::SignalType POSIX_physicalToLogical(int signal)
{
    switch (signal)
    {
    case SIGINT:    return SignalType_int;
    case SIGTERM:   return SignalType_term;
    case SIGHUP:    return SignalType_reload;
    case SIGSEGV:   return SignalType_segv;
    default:
        return SignalType_unhandled;
    }
}
#endif //_WIN32



#ifdef _WIN32
BOOL WINAPI WIN32_handleFunc(DWORD signal)
{
    if (nullptr != g_handler)
    {
        int signo = WIN32_physicalToLogical(signal);
        // The std::set is thread-safe in const reading access and we never
        // write to it after the program has started so we don't need to
        // protect this search by a mutex
        std::set<int>::const_iterator found = g_registry.find(signo);
        if (signo != -1 && found != g_registry.end())
        {
            return g_handler->handleSignal(signo) ? TRUE : FALSE;
        }
        else
        {
            return FALSE;
        }
    }
    else
    {
        return FALSE;
    }
}
#else
void POSIX_handleFunc(int signal)
{
    if (g_handler)
    {
        gremlin::SignalType signo = POSIX_physicalToLogical(signal);
        g_handler->handleSignal(signo);
    }
}
#endif //_WIN32

} // namespace drumlin
