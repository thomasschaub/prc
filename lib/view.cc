#include "view.h"

#include <iostream>

View::View() {
}

void View::setTime(unsigned v) {
    t = v;
}

void View::setWidth(unsigned v) {
    dt = v;
}

void View::draw(const Note& note) {
    // Check if note is out of screen
    auto effectiveEnd = note.end == 0 ? t : note.end;
    bool leftOut = effectiveEnd < t;
    bool rightOut = t + dt < note.start;
    if (leftOut || rightOut) {
        return;
    }

    std::cout << "Play " << note.note << " from " << note.start << " to " << effectiveEnd << std::endl;
}
