#include "view.h"

#include <algorithm>
#include <iostream>

View::View() {
}

void View::setTime(unsigned v) {
    t = v;
}

void View::setWidth(unsigned v) {
    dt = v;
}

void View::setCenter(float f) {
    center = std::max(0.0f, std::min(1.0f, f));
}

void View::draw(const Note& note) {
    // Compute displayed time span
    unsigned leftT = t - center*dt;
    unsigned rightT = t + (1 - center) * dt;

    // Check if note is out of screen
    auto effectiveEnd = note.end == 0 ? t : note.end;
    bool leftOut = effectiveEnd < leftT;
    bool rightOut = rightT < note.start;
    if (leftOut || rightOut) {
        return;
    }

    std::cout << "Play " << note.note << " from " << note.start << " to " << effectiveEnd << std::endl;
}
