#pragma once
#include <cstdint>
#include <SDL2/SDL.h>
#include "Paint.h"

class Main {
public:
    static int main();
    static uint32_t getWindowPixelFormat();

private:
    static void tick();
    static void draw(SDL_Renderer* rend);

    static SDL_Window* window;
    static SDL_Renderer* renderer;

    static Paint paint;
};