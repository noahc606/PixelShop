#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <nch/rmlui-utils/sdl-webview.h>
#include <nch/rmlui-utils/web-events.h>

class Window {
public:
    static void initRenderer(SDL_Renderer* rend);
    virtual void tick();
    virtual void draw();
    virtual void events(SDL_Event& evt);
    virtual void reload();

    void close();
    bool isAlive() { return !(decaying && age<=0); }
    uint64_t getAge() { return age; }
protected:
    void super_init(const std::string& webdocAssetPath, nch::Vec2i dims);
    void super_tick();
    void super_draw();

    int windowID = -1;
    int64_t age = 0; int64_t maxAge = 10;
    bool decaying = false;

    nch::SDL_Webview webview;
    nch::Vec2i pos = {100, 100};
    static int latestWindowID;
    static SDL_Renderer* rend;
private:
};