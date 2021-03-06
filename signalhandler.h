#ifndef SIGNALHANDLER_H
#define SIGNALHANDLER_H

namespace drumlin {

class SignalHandler
{
public:
    SignalHandler(int mask = DEFAULT_SIGNALS);
    virtual ~SignalHandler();

    enum SIGNALS
    {
        SIG_UNHANDLED   = 0,    // Physical signal not supported by this class
        SIG_NOOP        = 1,    // The application is requested to do a no-op (only a target that platform-specific signals map to when they can't be raised anyway)
        SIG_INT         = 2,    // Control+C (should terminate but consider that it's a normal way to do so; can delay a bit)
        SIG_TERM        = 4,    // Control+Break (should terminate now without regarding the consquences)
        SIG_CLOSE       = 8,    // Container window closed (should perform normal termination, like Ctrl^C) [Windows only; on Linux it maps to SIG_TERM]
        SIG_RELOAD      = 16,   // Reload the configuration [Linux only, physical signal is SIGHUP; on Windows it maps to SIG_NOOP]
        SIG_SEGV        = 32,
        SIG_PIPE        = 64,
        SIG_CHILD       = 128,
        DEFAULT_SIGNALS = SIG_INT | SIG_TERM | SIG_CLOSE | SIG_PIPE,
    };
    static const int numSignals = 9;

    virtual bool handleSignal(int signal);

private:
    int _mask;
};

} // namespace drumlin

#endif // SIGNALHANDLER_H
