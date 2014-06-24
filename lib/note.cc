#include "note.h"

Note::Note():
    pitch(0),
    velocity(0),
    start(-1),
    end(-1)
{
}

void Note::reset() {
    start = end = -1;
}
