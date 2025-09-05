#include "GUIs.h"
#include <tinyfiledialogs/tinyfiledialogs.h>
#include "ColorPicker.h"
#include "Window.h"

using namespace nch;

std::vector<Window*> GUIs::windows;

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
    for(int i = 0; i<windows.size(); i++) {
        windows[i]->tick();
    }
}
void GUIs::draw()
{
    for(int i = 0; i<windows.size(); i++) {
        windows[i]->draw();
    }
}

std::string GUIs::showFileDialogNative()
{
    char const* lFilterPatterns[] = { "*.png", "*.gif", "*.jpg" };

    // there is also a wchar_t version
    char const* selection = tinyfd_openFileDialog(
        "Select file", // title
        "", // optional initial directory
        2, // number of filter patterns
        lFilterPatterns, // char const * lFilterPatterns[2] = { "*.txt", "*.jpg" };
        NULL, // optional filter description
        0 // forbids multiple selections
    );
    printf("Selected the file \"%s\".\n", selection);
    return selection;
}

void GUIs::addColorPickerDialog(const Color& originalColor)
{
    addWindow(new ColorPicker());
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