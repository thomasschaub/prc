#include <SDL2/SDL.h>

#include "note.h"

class View {
public:
    View(SDL_Renderer* renderer);

    void setTime(unsigned t);
    void setWidth(unsigned dt);
    void setCenter(float c);

    void clear();
    void draw(const Note& note);
    void update();

private:
    int screenX(unsigned t);

    int width, height;

    SDL_Window* window;
    SDL_Renderer* renderer;

    unsigned t = 0;
    unsigned leftT = 0, rightT = 0;
    unsigned dt = 1000;
    float center = .5;
};
