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
    void drawHollow(const Note& note);
    void line(const Note& note);
    void finish();

    unsigned char minPitch, maxPitch;

private:
    int screenX(float t);
    float noteH();
    float noteY(float pitch);

    bool isOutOfScreen(float start, float end) const;

    int width, height;

    SDL_Window* window;
    SDL_Renderer* renderer;

    float t = 0;
    float leftT = 0, rightT = 0;
    float dt = 8;
    float center = .3;
};
