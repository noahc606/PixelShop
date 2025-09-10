#pragma once
#include <SDL2/SDL.h>
#include <cstdint>
#include "Paint.h"

class Main {
public:
    Main(); ~Main();

    static SDL_Renderer* getRenderer();
    static uint32_t getWindowPixelFormat();
    static int getWidth();
    static int getHeight();

private:
    static void tick();
    static void draw(SDL_Renderer* rend);
    static void events(SDL_Event& evt);

    static SDL_Window* window;
    static SDL_Renderer* renderer;
    static std::string basePath;
    static Paint* paint;
    static bool tickedYet;
};