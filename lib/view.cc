#include "view.h"

#include <algorithm>
#include <iostream>

View::View(SDL_Renderer* renderer):
    renderer(renderer)
{
    SDL_GetRendererOutputSize(renderer, &width, &height);
}

void View::setTime(float v) {
    t = v;
    leftT = t - center*dt;
    rightT = t + (1-center)*dt;
}

void View::setWidth(float v) {
    dt = v;
}

void View::setCenter(float f) {
    center = std::max(0.0f, std::min(1.0f, f));
}

void View::background() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
}

void View::draw(const Note& note) {
    // Check if note is out of screen
    auto effectiveEnd = note.end == 0 ? t : note.end;
    bool leftOut = effectiveEnd < leftT;
    bool rightOut = rightT < note.start;
    if (leftOut || rightOut) {
        return;
    }

    int screenL = std::max(0, screenX(note.start));
    int screenR = std::min(screenX(effectiveEnd), static_cast<int>(width));
    const float SCREEN_HEIGHT = height / 128.0f;
    int screenY = height - (note.note + 1) * SCREEN_HEIGHT;
    SDL_Rect r {
        screenL,
        screenY,
        screenR - screenL,
        static_cast<int>(SCREEN_HEIGHT)
    };
    SDL_RenderFillRect(renderer, &r);
}

void View::finish() {
    auto lineX = center * width;
    SDL_RenderDrawLine(renderer, lineX, 0, lineX, height);

    SDL_RenderPresent(renderer);
}

int View::screenX(float t) {
    return (t - leftT) / dt * width;
}
