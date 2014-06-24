#include "view.h"

#include <algorithm>
#include <cmath>
#include <iostream>

namespace {

/**
 * Puts b in the range [a, b]
 */
float clamp(float a, float b, float c) {
    if (b < a) {
        return a;
    }
    else if (c < b) {
        return c;
    }
    else {
        return b;
    }
}

}

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
    SDL_SetRenderDrawColor(renderer, 30, 30, 30, 255);
    SDL_RenderClear(renderer);

    // Draw lines of the staff

    const int C_INTENSITY = 200;
    const int IVORY_INTENSITY = 135;
    const int EBONY_INTENSITY = 80;

    float h = noteH();
    for (int i = 0; i < maxPitch - minPitch + 1; ++i) {
        int pitch = minPitch + i;
        switch (pitch % 12) {
        case 0:
            SDL_SetRenderDrawColor(
                renderer,
                C_INTENSITY,
                C_INTENSITY,
                C_INTENSITY,
                255
            );
            break;
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
        int y = height - (i + .5) * h;
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
    int screenY = noteY(note.pitch);
    SDL_Rect r {
        screenL,
        screenY,
        screenR - screenL,
        static_cast<int>(screenH)
    };
    SDL_RenderFillRect(renderer, &r);
}

void View::line(const Note& note) {
    auto y = noteY(note.pitch) + 0.5*noteH();
    SDL_RenderDrawLine(renderer, 0, y, width, y);
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
