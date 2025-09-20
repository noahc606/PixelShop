#include "ColorPicker.h"
#include <SDL2/SDL.h>
#include <nch/sdl-utils/texture-utils.h>
using namespace nch;

ColorPicker::ColorPicker(const Vec2i& dims)
{
    super_init("win_color_picker.html", dims);

    webEvents.trackEvent(WebEvent::CLICK, webview.getWorkingDocument()->GetElementById("close-btn"));

    SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormat(0, 100, 100, 32, SDL_PIXELFORMAT_RGBA8888);
    for(int ix = 0; ix<surf->w; ix++) {
    for(int iy = 0; iy<surf->h; iy++) {
        Color col; col.setFromHSV(120, ix, iy);
        TexUtils::setPixelColor(surf, ix, iy, col.getRGBA());
    }}
    if(colorPickerGraphic!=nullptr)
        colorPickerGraphic = SDL_CreateTextureFromSurface(rend, surf);
    SDL_FreeSurface(surf);

    auto doc = webview.getWorkingDocument();
    
}
ColorPicker::~ColorPicker()
{
    if(colorPickerGraphic!=nullptr)
        SDL_DestroyTexture(colorPickerGraphic);
    colorPickerGraphic = nullptr;
}

void ColorPicker::tick()
{
    super_tick();

    WebEvent wevt;
    while((wevt = webEvents.popEvent()).exists()) {
        if(wevt.getElementID()=="close-btn") {
            close();
        }
    }
}
void ColorPicker::draw()
{
    super_draw();
}
void ColorPicker::events(SDL_Event& evt)
{
    webview.events(evt);
}