#include <SDL2/SDL.h>

#include "note.h"

class View {
public:
    View(SDL_Renderer* renderer);

    void setTime(float t);
    void setWidth(float dt);
    void setCenter(float c);

    void background();
    void draw(const Note& note);
    void finish();

private:
    int screenX(float t);

    int width, height;

    SDL_Window* window;
    SDL_Renderer* renderer;

    float t = 0;
    float leftT = 0, rightT = 0;
    float dt = 8;
    float center = .5;
};
