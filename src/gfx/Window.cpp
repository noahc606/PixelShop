#include "Window.h"
#include <nch/sdl-utils/rect.h>
#include "Main.h"
using namespace nch;
int Window::latestWindowID = -1;
SDL_Renderer* Window::rend = nullptr;

void Window::initRenderer(SDL_Renderer* rend) {
    Window::rend = rend;
}

void Window::super_init(const std::string& webdocAssetPath, nch::Vec2i dims)
{
    latestWindowID++;
    windowID = latestWindowID;

    std::stringstream ctxID; ctxID << "generic_window_" << latestWindowID;
    webview.initContext(ctxID.str());
    webview.rmlLoadDocumentAsset(webdocAssetPath);
    webview.resize(dims);

    int mw = Main::getWidth(), mh = Main::getHeight();
    pos = {(mw-dims.x)/2, (mh-dims.y)/2};
}

void Window::tick(){}
void Window::super_tick()
{    
    if(!decaying) { age++; }
    else          { age--; }

    if(age<0) age = 0;
    if(age>maxAge) age = maxAge;

    webview.tick(pos);
}
void Window::draw(){}
void Window::super_draw()
{
    webview.render();
    if(age<maxAge) {
        Vec2i dims = webview.getDims();
        Rect dst = Rect(
            pos.x+dims.x/2-(dims.x*age)/(maxAge*2),
            pos.y+dims.y/2-(dims.y*age)/(maxAge*2),
            dims.x*(age/(float)maxAge),
            dims.y*(age/(float)maxAge)
        );
        webview.drawCopy(dst, age/(float)maxAge);
    } else {
        webview.drawCopy(pos);
    }
}

void Window::events(SDL_Event& evt){}
void Window::reload()
{
    webview.reload();
}

void Window::close() {
    decaying = true;
    age = maxAge;
}