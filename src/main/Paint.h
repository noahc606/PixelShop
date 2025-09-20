#pragma once
#include <nch/cpp-utils/color.h>
#include <nch/math-utils/vec2.h>
#include <nch/rmlui-utils/sdl-webview.h>
#include <nch/rmlui-utils/web-events.h>
#include <nch/sdl-utils/rect.h>
#include <SDL2/SDL.h>
#include "Canvas.h"


class Paint {
public:
    enum Tool {
        SELECTION,
        PENCIL,
        ERASER,
        FILL_BUCKET,
    };

    Paint(SDL_Renderer* rend, std::string openedImgPath = "");
    ~Paint();
    void tick();
    void draw(SDL_Renderer* rend);
    void reload();

private:
    void selectTool(int toolType);
    void setColorSquare(int id, nch::Color col);
    nch::Color selectColorSquare(std::string id);
    nch::Color selectColorSquare(int idNo);

    nch::SDL_Webview webview;
    nch::WebEventHolder webEvents;

    nch::Vec2i cursorPos = {0,0}, lastCursorPos = {-1,-1};

    Canvas* canv = nullptr;
    int holdingSave = 0;

    nch::Color toolColor;
    int toolType = PENCIL;
};