#include "Init.h"
#include <SDL2/SDL_filesystem.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <nch/rmlui-utils/sdl-webview.h>
using namespace nch;

void Init::libSDL(std::string& basePath, SDL_Window*& sdlWindow, SDL_Renderer*& sdlRenderer, SDL_PixelFormat*& pxFmt)
{
    int sdlFlags = SDL_INIT_VIDEO;
    if(SDL_Init(sdlFlags)!=0) {
        printf("SDL_Init error: %s\n", SDL_GetError());
    }
    basePath = SDL_GetBasePath();

    int imgFlags = IMG_INIT_AVIF | IMG_INIT_JPG | IMG_INIT_JXL | IMG_INIT_TIF | IMG_INIT_WEBP | IMG_INIT_PNG;
    IMG_Init(imgFlags);

    int winFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN;
    sdlWindow = SDL_CreateWindow("PixelShop", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, winFlags);
    if(sdlWindow==NULL) {
        printf("SDL_CreateWindow error: %s\n", SDL_GetError());
    }

    int rendFlags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE;
    sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, rendFlags);
    
    pxFmt = SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888);
}

void Init::libRmlUi(SDL_Renderer* sdlRenderer, std::string sdlBasePath)
{
    nch::SDL_Webview::rmlGlobalInit(sdlRenderer, sdlBasePath);
}