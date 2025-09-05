#pragma once
#include "Window.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <nch/cpp-utils/color.h>
#include <nch/rmlui-utils/sdl-webview.h>


class ColorPicker : public Window {
public:
    ColorPicker();
    ~ColorPicker();
    void tick();
    void draw();
    
private:
    SDL_Texture* colorPickerGraphic = nullptr;
};