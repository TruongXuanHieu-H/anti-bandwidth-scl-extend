#include "signal_handler.h"
#include <iostream>
#include <csignal>

void (*SignalHandler::signal_SIGINT)(int) = SIG_DFL;
void (*SignalHandler::signal_SIGXCPU)(int) = SIG_DFL;
void (*SignalHandler::signal_SIGSEGV)(int) = SIG_DFL;
void (*SignalHandler::signal_SIGTERM)(int) = SIG_DFL;
void (*SignalHandler::signal_SIGABRT)(int) = SIG_DFL;

const char *SignalHandler::signal_name(int signum)
{
    switch (signum)
    {
    case SIGINT:
        return "SIGINT";
    case SIGXCPU:
        return "SIGXCPU";
    case SIGSEGV:
        return "SIGSEGV";
    case SIGTERM:
        return "SIGTERM";
    case SIGABRT:
        return "SIGABRT";
    default:
        return "UNKNOWN";
    }
}

void SignalHandler::SIGINT_exit(int signum)
{
    signal(SIGINT, signal_SIGINT);
    signal(SIGXCPU, signal_SIGXCPU);
    signal(SIGSEGV, signal_SIGSEGV);
    signal(SIGTERM, signal_SIGTERM);
    signal(SIGABRT, signal_SIGABRT);

    std::cout << "c [Signal] Signal interruption. Caught signal: "
              << signal_name(signum) << " (" << signum << ").\n";

    fflush(stdout);
    fflush(stderr);

    raise(signum);
}

void SignalHandler::init_signals()
{
    signal_SIGINT = signal(SIGINT, SIGINT_exit);
    signal_SIGXCPU = signal(SIGXCPU, SIGINT_exit);
    signal_SIGSEGV = signal(SIGSEGV, SIGINT_exit);
    signal_SIGTERM = signal(SIGTERM, SIGINT_exit);
    signal_SIGABRT = signal(SIGABRT, SIGINT_exit);
}
