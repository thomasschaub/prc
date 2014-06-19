#ifndef MIDI_H__
#define MIDI_H__

#include <portmidi.h>

enum NoteEventType {
    ON,
    OFF
};

struct NoteEvent {
    char note;
    NoteEventType type;
    unsigned time;
};

int getNoteEvent(PmStream* stream, NoteEvent* buffer, unsigned n);

#endif
