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
    (SignalType_last)(33) \
)
ENUMN(SignalType,SignalTypes)

#define DEFAULT_SIGNALS ((int)gremlin::SignalType_int | (int)gremlin::SignalType_term | (int)gremlin::SignalType_close)

namespace drumlin {

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
