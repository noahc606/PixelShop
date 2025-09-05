#pragma once
#include <SDL2/SDL_events.h>
#include <cstdint>
#include <SDL2/SDL.h>
#include "Paint.h"

class Main {
public:
    Main(); ~Main();

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