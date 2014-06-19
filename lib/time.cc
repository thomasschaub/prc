#include "time.h"

#include <SDL2/SDL.h>

int now(void*) {
    return SDL_GetTicks();
}
