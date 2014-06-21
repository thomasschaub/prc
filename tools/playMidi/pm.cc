#include <portmidi.h>

int main() {
    Pm_Initialize();

    PortMidiStream* stream;
    PmDeviceID id = 6;
    Pm_OpenOutput(&stream, id, nullptr, 8, nullptr, nullptr, 0);

    Pm_WriteShort(stream, 0, 0x3b4190);

    Pm_Close(stream);

    Pm_Terminate();
}
