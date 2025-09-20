#pragma once
#include <SDL2/SDL.h>
#include <string>

class Init {
public:
    static void libSDL(std::string& basePath, SDL_Window*& sdlWindow, SDL_Renderer*& sdlRenderer, SDL_PixelFormat*& pxFmt);
    static void libRmlUi(SDL_Renderer* sdlRenderer, std::string sdlBasePath);
private:
};