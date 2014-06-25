#include "cli.h"

#include <iostream>
#include <string>

void listDevices() {
    int n = Pm_CountDevices();
    for (int i = 0; i < n; ++i) {
        const PmDeviceInfo* info = Pm_GetDeviceInfo(i);
        std::cout << i << ": " << info->name;
        if (info->input) {
            std::cout << " In";
        }
        if (info->output) {
            std::cout << " Out";
        }
        std::cout << std::endl;
    }
}

CliArgs readCliArgs(int argc, const char* argv[]) {
    CliArgs ret;

    for (int i = 1; i < argc; ++i) {
        std::string arg(argv[i]);
        if (arg == "-i") {
            if (i == argc - 1) {
                std::cerr << "-i requires an argument" << std::endl;
                abort();
            }
            ret.inputDevice = atoi(argv[++i]);
        }
        else if (arg == "-o") {
            if (i == argc - 1) {
                std::cerr << "-o requires an argument" << std::endl;
                abort();
            }
            ret.outputDevice = atoi(argv[++i]);
        }
        else if (arg == "-f") {
            ret.fullscreen = true;
        }
        else {
            ret.songPath = argv[i];
        }
    }

    return ret;
}
