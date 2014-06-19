#include "view.h"

#include <algorithm>
#include <iostream>

View::View() {
    SDL_CreateWindowAndRenderer(width, height, 0, &window, &renderer);
}

void View::setTime(unsigned v) {
    t = v;
    leftT = t - center*dt;
    rightT = t + (1-center)*dt;
}

void View::setWidth(unsigned v) {
    dt = v;
}

void View::setCenter(float f) {
    center = std::max(0.0f, std::min(1.0f, f));
}

void View::clear() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
}

void View::draw(const Note& note) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);

    // Check if note is out of screen
    auto effectiveEnd = note.end == 0 ? t : note.end;
    bool leftOut = effectiveEnd < leftT;
    bool rightOut = rightT < note.start;
    if (leftOut || rightOut) {
        return;
    }

    unsigned screenL = screenX(note.start);
    unsigned screenR = screenX(effectiveEnd);
    SDL_Rect r {
        static_cast<int>(screenL),
        static_cast<int>(height / 4),
        static_cast<int>(screenR - screenL),
        static_cast<int>(height / 2)
    };
    SDL_RenderFillRect(renderer, &r);
}

void View::update() {
    SDL_RenderPresent(renderer);
}

unsigned View::screenX(unsigned t) {
    return static_cast<float>(t - leftT) / dt * width;
}
