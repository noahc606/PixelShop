#include "Main.h"
#include <iostream>
#include <tinyfiledialogs/tinyfiledialogs.h>
#include <nch/cpp-utils/fs/FilePath.h>
#include <nch/sdl-utils/MainLoopDriver.h>

SDL_Window* Main::window = nullptr;
SDL_Renderer* Main::renderer = nullptr;
Paint Main::paint;

int main() { Main m; return m.main(); }

void initSDL(SDL_Window*& sdlWindow, SDL_Renderer*& sdlRenderer)
{
    int sdlFlags = SDL_INIT_VIDEO;
    if( SDL_Init(sdlFlags)!=0 ) {
        printf("SDL_Init error: %s\n", SDL_GetError());
    }

    int winFlags = SDL_WINDOW_RESIZABLE;
    sdlWindow = SDL_CreateWindow("PixelShop", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, winFlags);
    if(sdlWindow==NULL) {
        printf("SDL_CreateWindow error: %s\n", SDL_GetError());
    }
    
    int rendFlags = SDL_RENDERER_TARGETTEXTURE;
    sdlRenderer = SDL_CreateRenderer(sdlWindow, -1, rendFlags);
    
}

void fileDialog()
{
    char const* lFilterPatterns[] = { "*.png", "*.gif", "*.jpg" };

    // there is also a wchar_t version
    char const* selection = tinyfd_openFileDialog(
        "Select file", // title
        "", // optional initial directory
        2, // number of filter patterns
        lFilterPatterns, // char const * lFilterPatterns[2] = { "*.txt", "*.jpg" };
        NULL, // optional filter description
        0 // forbids multiple selections
    );
    printf("Selected the file \"%s\".\n", selection);
}

int Main::main()
{
    printf("Starting...\n");
    initSDL(window, renderer);
    SDL_ShowWindow(window);

    paint.init(renderer);    
    nch::MainLoopDriver mainLoop(renderer, &tick, 50, &draw, 200);

    return 0;
}

uint32_t Main::getWindowPixelFormat()
{
    return SDL_GetWindowPixelFormat(window);
}

void Main::tick()
{
    paint.tick();
}

void Main::draw(SDL_Renderer* rend)
{
	//Set render target to default and blendmode to blend
	SDL_SetRenderTarget(rend, NULL);
	SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
	
	//Black background
	SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
	SDL_RenderFillRect(rend, NULL);

    paint.draw(rend);

	//Render present objects on screen
	SDL_RenderPresent(rend);
}