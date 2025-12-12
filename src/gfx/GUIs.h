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

    //Working with 'windows'
    static std::string openFileDialog();
    static std::string saveFileDialog();
    static void addColorPickerPopup(const nch::Color& originalColor);
    static void addGenericPopup(const std::string& webdocAssetPath);
    static void setContextMenu(const nch::Vec2i& spawnPos, int itemSet);
    static void removeContextMenus();
    static bool doesWindowTypeExist(int windowType);
    static bool doesContextMenuExist();
    
    static Window* addWindow(Window* win);
    static void removeLastWindow();
    //Working with 'pendingEvents'
    static void addPendingEvent(std::string evtID);
    static bool tryPopPendingEvent(std::string evtID);

private:

    static std::vector<Window*> windows;
    static std::set<std::string> pendingEvents;
};