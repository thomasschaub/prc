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

    const int IVORY_INTENSITY = 135;
    const int EBONY_INTENSITY = 80;

    float h = noteH();
    for (int i = 0; i < maxPitch - minPitch + 1; ++i) {
        int pitch = minPitch + i;
        switch (pitch % 12) {
        case 1:
        case 3:
        case 6:
        case 8:
        case 10:
            SDL_SetRenderDrawColor(
                renderer,
                EBONY_INTENSITY,
                EBONY_INTENSITY,
                EBONY_INTENSITY,
                255
            );
            break;
        default:
            SDL_SetRenderDrawColor(
                renderer,
                IVORY_INTENSITY,
                IVORY_INTENSITY,
                IVORY_INTENSITY,
                255
            );
        }
        int y = (i + .5) * h;
        SDL_RenderDrawLine(renderer, 0, y, width, y);
    }
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
    float screenH = noteH();
    int screenY = noteY(note.note);
    SDL_Rect r {
        screenL,
        screenY,
        screenR - screenL,
        static_cast<int>(screenH)
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

float View::noteH() {
    return static_cast<float>(height) / (maxPitch - minPitch + 1);
}

float View::noteY(float pitch) {
    return (1 - (pitch - minPitch) / (maxPitch - minPitch)) * (height - noteH());
}
