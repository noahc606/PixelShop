#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <nch/rmlui-utils/sdl-webview.h>

class Window {
public:
    static void initRenderer(SDL_Renderer* rend);
    virtual void tick();
    virtual void draw();
    virtual void reload();

protected:
    void super_init(const std::string& webdocAssetPath, nch::Vec2i dims);

    nch::SDL_Webview webview;
    nch::Vec2i pos = {100, 100};
    static int latestWindowID;
    static SDL_Renderer* rend;
private:
};