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

    int screenL = screenX(note.start);
    int screenR = screenX(effectiveEnd);
    const int SCREEN_HEIGHT = height / 128;
    int screenY = note.note * SCREEN_HEIGHT;
    SDL_Rect r {
        screenL,
        screenY,
        screenR - screenL,
        SCREEN_HEIGHT,
    };
    SDL_RenderFillRect(renderer, &r);
}

void View::update() {
    SDL_RenderPresent(renderer);
}

int View::screenX(unsigned t) {
    return static_cast<float>(t - leftT) / dt * width;
}
