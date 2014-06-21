#include "midi.h"

#include <cstdlib>

#include <SDL2/SDL.h>

#if 0
static void printName(int id) {
    const PmDeviceInfo* info = Pm_GetDeviceInfo(id);
    std::cout << info->name << " " << info->input << std::endl;
}
#endif

NoteEventType getType(const PmEvent& e) {
    int status = e.message % 0x100;
    // 0x8n is note off on channel n
    // 0x9n is note on on channel n
    switch (status) {
    case 0x80:
        return OFF;
    case 0x90:
        return ON;
    default:
        abort();
    }
}

char getNote(const PmEvent& e) {
    return (e.message >> 8) % 256;
}

int getNoteEvent(PmStream* stream, NoteEvent* buffer, unsigned n) {
    PmEvent pmBuffer[n];
    int pmN = Pm_Read(stream, pmBuffer, n);
    for (int i = 0; i < pmN; ++i) {
        NoteEvent& note = buffer[i];
        note.type = getType(pmBuffer[i]);
        note.note = getNote(pmBuffer[i]);
        note.time = pmBuffer[i].timestamp;
        SDL_LogDebug(SDL_LOG_CATEGORY_INPUT, "Note %d %d %d\n", note.type, note.note, note.time);
        SDL_LogDebug(SDL_LOG_CATEGORY_INPUT, "Raw msg: %x\n", pmBuffer[i].message);
    }
    return pmN;
}

