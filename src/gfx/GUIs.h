#pragma once
#include <nch/cpp-utils/color.h>
#include <nch/rmlui-utils/sdl-webview.h>
#include <nch/rmlui-utils/web-events.h>
#include <vector>
#include "Window.h"

class GUIs {
public:
    static void globalInit(SDL_Renderer* rend);
    static void globalReload();
    static void globalFree();
    static void tick();
    static void draw();
    static void events(SDL_Event& evt);

    static std::string showFileDialogNative();
    static void addColorPickerDialog(const nch::Color& originalColor);
    static Window* addWindow(Window* win);
    static void removeLastWindow();

private:

    static std::vector<Window*> windows;
};