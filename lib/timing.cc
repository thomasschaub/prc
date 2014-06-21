#include "timing.h"

#include <SDL2/SDL.h>

namespace {

float convertBeats(float bpm) {
    return bpm / (60 * 1000);
}

float beatsPerMs = convertBeats(120);
float beatT;

}

void setBpm(float bpm) {
    beatsPerMs = convertBeats(bpm);
}

float beatTime() {
    return beatT;
}

void updateBeatTime(float s) {
    // Gather last and current wall time
    static int lastWallT = SDL_GetTicks();
    int wallT = SDL_GetTicks();

    // Update beat time
    int dWallT = wallT - lastWallT;
    beatT += dWallT * beatsPerMs;

    lastWallT = wallT;
}

int wallTime(void*) {
    return SDL_GetTicks();
}
