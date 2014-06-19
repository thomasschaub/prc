#include <SDL2/SDL.h>

#include "note.h"

class View {
public:
    View();

    void setTime(unsigned t);
    void setWidth(unsigned dt);
    void setCenter(float c);

    void draw(const Note& note);

private:
    unsigned t = SDL_GetTicks();
    unsigned dt = 1000;
    float center = .5;
};
