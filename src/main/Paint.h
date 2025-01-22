#pragma once
#include <SDL2/SDL.h>

class Paint {
public:
    void init(SDL_Renderer* rend);
    void tick();
    void draw(SDL_Renderer* rend);

private:
    void drawPixel(SDL_Renderer* rend, int px, int py);

    int mousePX = 0; int mousePY = 0;
    int lastMousePX = -1; int lastMousePY = -1;

    SDL_Texture* canvas = nullptr;
    SDL_Rect canvasRect;

    int holdingSave = 0;
};