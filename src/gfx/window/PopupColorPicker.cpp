#include "PopupColorPicker.h"
#include <SDL2/SDL.h>
#include <nch/sdl-utils/texture-utils.h>
using namespace nch;

PopupColorPicker::PopupColorPicker(const Vec2i& dims)
{
    super_init(Window::POPUP_COLOR_PICKER, dims);

    {
        SDL_Surface* surf = SDL_CreateRGBSurfaceWithFormat(0, 100, 100, 32, SDL_PIXELFORMAT_RGBA8888);
        for(int ix = 0; ix<surf->w; ix++) {
        for(int iy = 0; iy<surf->h; iy++) {
            Color col; col.setFromHSV(120, ix, iy);
            TexUtils::setPixelColor(surf, ix, iy, col.getRGBA());
        }}
        if(colorPickerGraphic!=nullptr)
            colorPickerGraphic = SDL_CreateTextureFromSurface(rend, surf);
        SDL_FreeSurface(surf);
    }


    reload();
}
PopupColorPicker::~PopupColorPicker()
{
    if(colorPickerGraphic!=nullptr)
        SDL_DestroyTexture(colorPickerGraphic);
    colorPickerGraphic = nullptr;
}

void PopupColorPicker::reload()
{
    super_reload();
}
void PopupColorPicker::tick()
{
    super_tick();
}
void PopupColorPicker::draw()
{
    super_draw();
}