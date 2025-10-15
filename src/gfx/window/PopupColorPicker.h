#pragma once
#include "Window.h"
#include <SDL2/SDL.h>
#include <nch/cpp-utils/color.h>
#include <nch/math-utils/vec2.h>
#include <nch/rmlui-utils/sdl-webview.h>
#include <nch/rmlui-utils/web-events.h>

class PopupColorPicker : public Window {
public:
    PopupColorPicker(const nch::Vec2i& dims = {480, 360});
    ~PopupColorPicker();
    void reload();
    void tick();
    void draw();
    
private:
    SDL_Texture* colorPickerGraphic = nullptr;
};