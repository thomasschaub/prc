#ifndef MIDI_H__
#define MIDI_H__

#include <portmidi.h>

enum NoteEventType {
    ON,
    OFF
};

struct NoteEvent {
    unsigned char channel;
    unsigned char pitch;
    unsigned char velocity;
    NoteEventType type;
    float time;
};

int getNoteEvent(PmStream* stream, NoteEvent* buffer, unsigned n);

void putNoteEvent(PmStream* stream, const NoteEvent& e);

void putAllOff(PmStream* stream, unsigned char channel);

#endif
