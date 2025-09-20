#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_pixels.h>
#include <string>
#include "Paint.h"

class Main {
public:
    Main(); ~Main();

    static SDL_Renderer* getRenderer();
    static SDL_PixelFormat* getPixelFormat();
    static int getWidth();
    static int getHeight();
    static std::string getTimeFmtted();

private:
    static void tick();
    static void draw(SDL_Renderer* rend);
    static void events(SDL_Event& evt);

    static SDL_Window* window;
    static SDL_Renderer* renderer;
    static SDL_PixelFormat* pxFmt;
    static std::string basePath;
    static Paint* paint;
    static bool tickedYet;
};