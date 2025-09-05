#include "ColorPicker.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <nch/sdl-utils/texture-utils.h>
using namespace nch;

ColorPicker::ColorPicker()
{
    super_init("win_color_picker.html", {640, 480});

    SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormat(0, 100, 100, 32, SDL_PIXELFORMAT_RGBA8888);
    for(int ix = 0; ix<surf->w; ix++) {
    for(int iy = 0; iy<surf->h; iy++) {
        Color col; col.setFromHSV(120, ix, iy);
        TexUtils::setPixelColor(surf, ix, iy, col.getRGBA());
    }}
    if(colorPickerGraphic!=nullptr)
        colorPickerGraphic = SDL_CreateTextureFromSurface(rend, surf);
}
ColorPicker::~ColorPicker(){}

void ColorPicker::tick()
{
    webview.tick();
}
void ColorPicker::draw()
{
    webview.render();
    webview.drawCopy(pos);
}