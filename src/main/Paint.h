#pragma once
#include <nch/cpp-utils/color.h>
#include <nch/math-utils/vec2.h>
#include <nch/rmlui-utils/sdl-webview.h>
#include <nch/rmlui-utils/web-events.h>
#include <nch/sdl-utils/rect.h>
#include <SDL2/SDL.h>


class Paint {
public:
    Paint(SDL_Renderer* rend, std::string path = "");
    ~Paint();
    void tick();
    void draw(SDL_Renderer* rend);
    void reload();

private:
    void drawPixel(SDL_Renderer* rend, nch::Vec2i pos);
    void setColorSquare(int id, nch::Color col);
    nch::Color selectColorSquare(std::string id);
    nch::Color selectColorSquare(int idNo);

    nch::SDL_Webview webview;
    nch::WebEventHolder webEvents;

    nch::Rect workspace = nch::Rect(0, 0, 1, 1);
    nch::Vec2i cursorPos = {0,0};
    nch::Vec2i lastCursorPos = {-1,-1};

    SDL_Texture* canvas = nullptr;
    nch::Vec2i cvsPos = {0, 0};
    nch::Vec2i cvsDims = {1, 1};
    double cvsZoom = 5.0;

    int holdingSave = 0;

    nch::Color brushColor;
};