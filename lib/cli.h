#ifndef CLI_H__
#define CLI_H__

#include <portmidi.h>

struct CliArgs {
    const char* songPath = nullptr;
    PmDeviceID inputDevice = -1;
    PmDeviceID outputDevice = -1;
};

void listDevices();

CliArgs readCliArgs(int argc, const char* argv[]);

#endif
