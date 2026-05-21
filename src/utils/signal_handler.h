#ifndef SIGNAL_HANDLER_H
#define SIGNAL_HANDLER_H

#include <signal.h>

class SignalHandler
{
public:
    static void SIGINT_exit(int signum);
    static void init_signals();
    static const char *signal_name(int signum);

    static void (*signal_SIGINT)(int);
    static void (*signal_SIGXCPU)(int);
    static void (*signal_SIGSEGV)(int);
    static void (*signal_SIGTERM)(int);
    static void (*signal_SIGABRT)(int);
};

#endif // SIGNAL_HANDLER_H
