#ifndef CLI_H__
#define CLI_H__

#include <portmidi.h>

enum Mode {
    Mode_Play,
    Mode_Training
};

struct CliArgs {
    const char* songPath = nullptr;
    PmDeviceID inputDevice = -1;
    PmDeviceID outputDevice = -1;

    bool fullscreen = false;

    Mode mode = Mode_Training;
};

void listDevices();

CliArgs readCliArgs(int argc, const char* argv[]);

#endif
