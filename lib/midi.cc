#include "midi.h"

#include <cstdlib>

#include <SDL2/SDL.h>

#include "timing.h"

namespace {

NoteEvent convert(const PmEvent& e) {
    auto msg = e.message;
    NoteEvent ret;
    // channel = e.message % 0x10
    switch ((msg >> 4) % 0x10) {
        case 0x8:
            ret.type = OFF;
            break;
        case 0x9:
            ret.type = ON;
            break;
        default:
            abort();
    }
    ret.pitch = (msg >> 8) % 0x100;
    ret.velocity = (msg >> 16) % 0x100;
    return ret;
}

PmEvent convert(const NoteEvent& e) {
    PmEvent ret = {0};
    switch (e.type) {
        case ON:
            ret.message = 0x90;
            break;
        case OFF:
            ret.message = 0x80;
            break;
    }
    ret.message |= e.pitch << 8;
    ret.message |= e.velocity << 16;
    return ret;
}

}

int getNoteEvent(PmStream* stream, NoteEvent* buffer, unsigned n) {
    PmEvent pmBuffer[n];
    int pmN = Pm_Read(stream, pmBuffer, n);
    for (int i = 0; i < pmN; ++i) {
        NoteEvent& e = buffer[i];
        e = convert(pmBuffer[i]);
        e.time = beatTime();
        //SDL_LogDebug(SDL_LOG_CATEGORY_INPUT, "Note %d %d %d\n", note.type, note.note, note.time);
        SDL_LogDebug(SDL_LOG_CATEGORY_INPUT, "Raw msg: %x\n", pmBuffer[i].message);
    }
    return pmN;
}

void putNoteEvent(PmStream* stream, const NoteEvent& e) {
    PmEvent pmE = convert(e);
    SDL_LogDebug(SDL_LOG_CATEGORY_INPUT, "Send: %x\n", pmE.message);
    Pm_Write(stream, &pmE, 1);
}
