#include "GUIs.h"
#include <nch/cpp-utils/log.h>
#include <stdexcept>
#include <tinyfiledialogs/tinyfiledialogs.h>
#include "ContextMenu.h"
#include "PopupColorPicker.h"
#include "PopupGeneric.h"
#include "Window.h"

using namespace nch;

std::vector<Window*> GUIs::windows;
std::set<std::string> GUIs::pendingEvents;

void GUIs::globalInit(SDL_Renderer* rend) {
    Window::initRenderer(rend);
}
void GUIs::globalReload() {
    for(int i = 0; i<windows.size(); i++) { windows[i]->reload(); }
}
void GUIs::globalFree()
{
    while(windows.size()>0)
        removeLastWindow();
}

void GUIs::tick()
{
    for(int i = windows.size()-1; i>=0; i--) {
        if(!windows[i]->isAlive()) {
            delete windows[i];
            windows.erase(windows.begin()+i);
        }
    }
    for(int i = 0; i<windows.size(); i++) { windows[i]->tick(); }
}
void GUIs::draw()
{
    for(int i = 0; i<windows.size(); i++) { windows[i]->draw(); }
}

void GUIs::events(SDL_Event& evt)
{
    for(int i = 0; i<windows.size(); i++) { windows[i]->events(evt); }
}

std::string GUIs::openFileDialog()
{
    char const* lFilterPatterns[] = { "*.png", "*.jpg" };

    // there is also a wchar_t version
    char const* selection = tinyfd_openFileDialog(
        "Select file", // title
        "", // optional initial directory
        2, // number of filter patterns
        lFilterPatterns, // char const * lFilterPatterns[2] = { "*.txt", "*.jpg" };
        NULL, // optional filter description
        0 // forbids multiple selections
    );

    if(selection==nullptr) {
        throw std::invalid_argument("User pressed \"X\" on the dialog window.");
    }
    printf("Selected the file \"%s\".\n", selection);
    return selection;
}
std::string GUIs::saveFileDialog()
{
    char const* lFilterPatterns[] = { "*.png" };

    std::string ret = tinyfd_saveFileDialog(
        "Select save location",
        "",
        1,
        lFilterPatterns,
        "PNG file"
    );

    return ret;
}

void GUIs::addColorPickerPopup(const Color& originalColor)
{
    addWindow(new PopupColorPicker());
}

void GUIs::setContextMenu(const Vec2i& spawnPos, int itemSet)
{
    removeContextMenus();
    addWindow(new ContextMenu(spawnPos, itemSet));
}
void GUIs::removeContextMenus()
{
    for(int i = 0; i<windows.size(); i++) {
        if(windows[i]->getType()==Window::CONTEXT_MENU) {
            windows[i]->close();
        }
    }
}

bool GUIs::doesWindowTypeExist(int windowType)
{
    for(int i = 0; i<windows.size(); i++) {
        if(windows[i]->getType()==windowType && windows[i]->isAlive()) {
            return true;
        }
    }
    return false;
}
bool GUIs::doesContextMenuExist() {
    return doesWindowTypeExist(Window::CONTEXT_MENU);
}

void GUIs::addGenericPopup(const std::string& webdocAssetPath)
{
    addWindow(new PopupGeneric(webdocAssetPath));
}

Window* GUIs::addWindow(Window* win)
{
    windows.push_back(win);
    return win;
}
void GUIs::removeLastWindow()
{
    if(windows.size()==0) return;

    delete windows.at(windows.size()-1);
    windows.erase(windows.end());
}

void GUIs::addPendingEvent(std::string evtID)
{
    pendingEvents.insert(evtID);
}
bool GUIs::tryPopPendingEvent(std::string evtID)
{
    auto peItr = pendingEvents.find(evtID);
    if(peItr==pendingEvents.end()) {
        return false;
    }
    pendingEvents.erase(peItr);
    return true;
}