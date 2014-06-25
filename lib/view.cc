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

const int NOTE_BORDER_WIDTH = 2;

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

    const int IVORY_INTENSITY = 100;
    const int C_INTENSITY = IVORY_INTENSITY + 20;
    const int EBONY_INTENSITY = 60;

    float h = noteH();
    for (int i = maxPitch - minPitch; i >= 0; --i) {
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
        int y = height - (i + 1) * h;
        SDL_Rect rect {
            0,
            y,
            width,
            static_cast<int>(h) + 1
        };
        SDL_RenderFillRect(renderer, &rect);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderDrawLine(renderer, 0, y, width, y);
    }
}

void View::draw(const Note& note) {
    // Check if note is out of screen
    auto effectiveEnd = note.end == -1 ? t : note.end;
    if (isOutOfScreen(note.start, effectiveEnd)) {
        return;
    }

    int screenL = std::max(0, screenX(note.start));
    int screenR = std::min(screenX(effectiveEnd), static_cast<int>(width));
    float screenH = noteH();
    int screenY = noteY(note.pitch);
    SDL_Rect r {
        screenL,
        screenY + NOTE_BORDER_WIDTH,
        screenR - screenL,
        static_cast<int>(screenH) - 2*NOTE_BORDER_WIDTH
    };
    SDL_RenderFillRect(renderer, &r);
}

void View::drawHollow(const Note& note) {
    // Check if note is out of screen
    auto effectiveEnd = note.end == -1 ? t : note.end;
    if (isOutOfScreen(note.start, effectiveEnd)) {
        return;
    }

    int screenL = std::max(0, screenX(note.start));
    int screenR = std::min(screenX(effectiveEnd), static_cast<int>(width));
    float screenH = noteH();
    int screenY = noteY(note.pitch);

    // Draw left bar
    {
    SDL_Rect r {
        screenL,
        screenY,
        NOTE_BORDER_WIDTH,
        static_cast<int>(screenH)
    };
    SDL_RenderFillRect(renderer, &r);
    }

    // Draw right bar
    {
    SDL_Rect r {
        screenR - NOTE_BORDER_WIDTH,
        screenY,
        NOTE_BORDER_WIDTH,
        static_cast<int>(screenH)
    };
    SDL_RenderFillRect(renderer, &r);
    }

    // Draw top bar
    {
    SDL_Rect r {
        screenL,
        screenY,
        screenR - screenL,
        NOTE_BORDER_WIDTH,
    };
    SDL_RenderFillRect(renderer, &r);
    }

    // Draw bot bar
    {
    SDL_Rect r {
        screenL,
        screenY + static_cast<int>(screenH) - NOTE_BORDER_WIDTH,
        screenR - screenL,
        NOTE_BORDER_WIDTH,
    };
    SDL_RenderFillRect(renderer, &r);
    }
}

void View::line(const Note& note) {
    int y = noteY(note.pitch);
    SDL_Rect rect {
        0,
        y + NOTE_BORDER_WIDTH,
        width,
        static_cast<int>(noteH()) - 2*NOTE_BORDER_WIDTH
    };

    SDL_RenderFillRect(renderer, &rect);
}

void View::finish() {
    // Draw metronome
    {
    float highlightTime = 1.0f;
    double temp;
    float timeSinceBeat = clamp(0, modf(t, &temp), highlightTime);
    float metronomeSize = pow((highlightTime - timeSinceBeat) / highlightTime, 3);

    int metronomeX = width - 50;
    int metronomeY = 50;
    int metronomeW = 20 * metronomeSize;
    int metronomeH = metronomeW;
    SDL_Rect metronomeRect {
        metronomeX - metronomeW,
        metronomeY - metronomeH,
        2*metronomeW,
        2*metronomeH
    };
    SDL_RenderFillRect(renderer, &metronomeRect);
    }

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

bool View::isOutOfScreen(float start, float end) const {
    bool leftOut = end < leftT;
    bool rightOut = rightT < start;
    return leftOut || rightOut;
}
