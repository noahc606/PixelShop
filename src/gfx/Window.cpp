#include "Window.h"
#include <nch/sdl-utils/rect.h>
#include "ContextMenu.h"
#include "PopupGeneric.h"
#include "Main.h"
using namespace nch;
int Window::latestID = -1;
SDL_Renderer* Window::rend = nullptr;

void Window::initRenderer(SDL_Renderer* rend) {
    Window::rend = rend;
}

void Window::super_init(int type, nch::Vec2i dims)
{
    //Set 'type', 'latestID', and 'id'
    Window::type = type;
    latestID++; id = latestID;

    //Set 'webdocAssetPath'
    std::string webdocAssetPath = "";
    switch (type) {
        case POPUP_GENERIC: {
            PopupGeneric* thisPG = (PopupGeneric*)this;
            webdocAssetPath = thisPG->getWebdocAssetPath();
        } break;
        case POPUP_COLOR_PICKER: { webdocAssetPath = "window/color_picker.html"; } break;
        case CONTEXT_MENU: { webdocAssetPath = "window/context_menu.html"; } break;
        default: { webdocAssetPath = "window/generic.html"; } break;
    }

    //Setup 'webview'
    std::stringstream ctxID; ctxID << "generic_window_" << latestID;
    webview.initContext(ctxID.str());
    webview.rmlLoadDocumentAsset(webdocAssetPath);
    webview.resize(dims);

    //Set pos
    switch(type) {
        case CONTEXT_MENU: {
            ContextMenu* thisCM = (ContextMenu*)this;
            pos = thisCM->getSpawnPos();
        } break;
        case POPUP_GENERIC:
        case POPUP_COLOR_PICKER: {
            int mw = Main::getWidth(), mh = Main::getHeight();
            pos = {(mw-dims.x)/2, (mh-dims.y)/2};
        } break;
        default: {
            pos = {0, 0};
        } break;
    }
}

void Window::reload(){}
void Window::super_reload() {
    webview.reload();
    webEvents.trackEvent(WebEvent::CLICK, webview.getWorkingDocument()->GetElementById("close-btn"));
}
void Window::tick(){}
void Window::super_tick()
{
    WebEvent wevt;
    while((wevt = webEvents.popEvent()).exists()) {
        if(wevt.getElementID()=="close-btn") {
            close();
        }
    }

    //Handle decaying and age
    if(!decaying) { age++; }
    else          { age--; }
    if(age<0) age = 0;
    if(age>maxAge) age = maxAge;

    //Tick only if not decaying
    if(!decaying) {
        webview.setScreenPos(pos);
        webview.tick();
    }
}
void Window::draw(){}
void Window::super_draw()
{
    webview.render();

    //Draw window with different size ("shrinking/growing" behavior when closing/opening most windows)
    Rect dst;
    Vec2i dims = webview.getDims();
    if(type==GENERIC || type==POPUP_GENERIC || type==POPUP_COLOR_PICKER) {
        dst = Rect(
            pos.x+dims.x/2-(dims.x*age)/(maxAge*2),
            pos.y+dims.y/2-(dims.y*age)/(maxAge*2),
            dims.x*(age/(float)maxAge),
            dims.y*(age/(float)maxAge)
        );
    } else {
        dst = Rect(pos.x, pos.y, dims.x, dims.y);
    }

    if(age<maxAge) {
        webview.drawCopy(dst, age/(float)maxAge);
    } else {
        webview.drawCopy(pos);
    }
}
void Window::events(SDL_Event& evt){}

void Window::close() {
    if(!decaying) {
        decaying = true;
        age = maxAge;
    }
}
int Window::getType() {
    return type;
}
bool Window::isAlive() {
    return !(decaying && age<=0);
}
uint64_t Window::getAge() {
    return age;
}