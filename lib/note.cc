#include "note.h"

bool Note::operator<(const Note& rhs) const {
    return start < rhs.start;
}

void Note::reset() {
    start = end = -1;
}
