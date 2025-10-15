#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <nch/rmlui-utils/sdl-webview.h>
#include <nch/rmlui-utils/web-events.h>

class Window {
public:
    enum WindowType {
        GENERIC,
        CONTEXT_MENU,
        POPUP_GENERIC,
        POPUP_COLOR_PICKER,
    };

    static void initRenderer(SDL_Renderer* rend);
    virtual void reload();
    virtual void tick();
    virtual void draw();
    virtual void events(SDL_Event& evt);

    void close();
    int getType();
    bool isAlive();
    uint64_t getAge();
protected:
    void super_init(int type, nch::Vec2i dims);
    void super_reload();
    void super_tick();
    void super_draw();

    int type = GENERIC;
    int id = -1;
    int64_t age = 0; int64_t maxAge = 10;
    bool decaying = false;

    nch::SDL_Webview webview;
    nch::WebEventHolder webEvents;
    nch::Vec2i pos = {100, 100};
    static int latestID;
    static SDL_Renderer* rend;
private:
};