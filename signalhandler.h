#ifndef SIGNALHANDLER_H
#define SIGNALHANDLER_H

#include <string>
#include "metatypes.h"
#include "gtypes.h"

#define ST_NOOP
#define ST_INT
#define ST_TERM
#define ST_CLOSE
#define ST_RELOAD
#define ST_SEGV
#define ST_LAST

#define SignalTypes ( \
    (SignalType_first)(-1), \
    (SignalType_unhandled)(0), \
    (SignalType_noop)(1), \
    (SignalType_int)(2), \
    (SignalType_term)(4), \
    (SignalType_close)(8), \
    (SignalType_reload)(16), \
    (SignalType_segv)(32), \
    (SignalType_all)(63), \
    (SignalType_last)(33) \
)
ENUMN(SignalType,SignalTypes)

#define DEFAULT_SIGNALS ((int)gremlin::SignalType_int | (int)gremlin::SignalType_term | (int)gremlin::SignalType_close)

namespace drumlin {

#ifdef _WIN32

BOOL WINAPI WIN32_handleFunc(DWORD);
gremlin::SignalType WIN32_physicalToLogical(DWORD);
DWORD WIN32_logicalToPhysical(gremlin::SignalType);
std::set<gremlin::SignalType> g_registry;

#else // POSIX

extern void POSIX_handleFunc(int);
extern gremlin::SignalType POSIX_physicalToLogical(int);
extern int POSIX_logicalToPhysical(gremlin::SignalType);

#endif //_WIN32

class SignalHandler
{
public:
    SignalHandler(int mask = DEFAULT_SIGNALS);
    virtual ~SignalHandler();
    static const int numSignals = 5;

    virtual bool handleSignal(gremlin::SignalType signal);

private:
    int _mask;
};

} // namespace drumlin

#endif // SIGNALHANDLER_H
