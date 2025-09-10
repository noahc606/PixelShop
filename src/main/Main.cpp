#include "Main.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <nch/cpp-utils/filepath.h>
#include <nch/sdl-utils/input.h>
#include <nch/sdl-utils/main-loop-driver.h>
#include "GUIs.h"
#include "Init.h"

using namespace nch;

SDL_Window* Main::window = nullptr;
SDL_Renderer* Main::renderer = nullptr;
std::string Main::basePath = "";
Paint* Main::paint = nullptr;
bool Main::tickedYet = false;

int main() { Main m; return 0; }
Main::Main()
{
    printf("Starting...\n");
    Init::libSDL(basePath, window, renderer);
    Init::libRmlUi(renderer, basePath);
    GUIs::globalInit(renderer);

    std::string openedPath = "";
    #ifndef EMSCRIPTEN
        openedPath = GUIs::showFileDialogNative();
    #endif

    paint = new Paint(renderer, openedPath);
    MainLoopDriver mld(renderer, &tick, 50, &draw, 200, events);
}
Main::~Main()
{
    GUIs::globalFree();

    SDL_Webview::rmlGlobalShutdown();

    SDL_Quit();
    IMG_Quit();
}

SDL_Renderer* Main::getRenderer() {
    return renderer;
}
uint32_t Main::getWindowPixelFormat()
{
    return SDL_GetWindowPixelFormat(window);
}

int Main::getWidth() {
    int ret; SDL_GetWindowSize(window, &ret, NULL); return ret;
}
int Main::getHeight() {
    int ret; SDL_GetWindowSize(window, NULL, &ret); return ret;
}

void Main::tick()
{
    if(paint!=nullptr) {
        paint->tick();
        tickedYet = true;
    }

    if(Input::keyDownTime(SDLK_F5)==1) {
        GUIs::globalReload();
    }
    GUIs::tick();
}

void Main::draw(SDL_Renderer* rend)
{	
	//Black background
	SDL_SetRenderDrawColor(rend, 0, 0, 0, 255);
	SDL_RenderFillRect(rend, NULL);

    if(paint!=nullptr) {
        if(tickedYet)
            paint->draw(rend);
    }

    GUIs::draw();

	//Render present objects on screen
	SDL_RenderPresent(rend);
}

void Main::events(SDL_Event& evt)
{

}