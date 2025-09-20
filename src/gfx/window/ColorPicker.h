#pragma once
#include "Window.h"
#include <SDL2/SDL.h>
#include <nch/cpp-utils/color.h>
#include <nch/math-utils/vec2.h>
#include <nch/rmlui-utils/sdl-webview.h>
#include <nch/rmlui-utils/web-events.h>


class ColorPicker : public Window {
public:
    ColorPicker(const nch::Vec2i& dims = {480, 360});
    ~ColorPicker();
    void tick();
    void draw();
    void events(SDL_Event& evt);
    
private:
    SDL_Texture* colorPickerGraphic = nullptr;
    nch::WebEventHolder webEvents;
};