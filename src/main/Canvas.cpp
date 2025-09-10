#include "Canvas.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>
#include <nch/cpp-utils/fs-utils.h>
#include <nch/cpp-utils/log.h>
#include <nch/sdl-utils/input.h>
#include <nch/sdl-utils/rect.h>
#include <nch/sdl-utils/texture-utils.h>
#include "Main.h"
using namespace nch;

Canvas::Canvas(SDL_Renderer* rend, const std::string& path)
{
    Canvas::rend = rend;

    try {
        if(path=="") throw std::exception();
        if(!FsUtils::fileExists(path)) throw std::exception();
        
        //Create surface 'surfImg' from loading the image @ the provided 'path'.
        SDL_Surface* surfImg = IMG_Load(path.c_str());
        if(surfImg==nullptr) throw std::exception();

        //Create empty 'canvas' with same size as 'surfImg'
        tex = SDL_CreateTexture(rend, Main::getWindowPixelFormat(), SDL_TEXTUREACCESS_TARGET, surfImg->w, surfImg->h);
        if(tex==nullptr) throw std::exception();
        TexUtils::clearTexture(rend, tex);

        //Create texture 'texImg' from 'surfImg'
        SDL_Texture* texImg = SDL_CreateTextureFromSurface(rend, surfImg);
        SDL_FreeSurface(surfImg);
        if(texImg==nullptr) throw std::exception();
        //Copy loaded 'texImg' to canvas
        SDL_SetRenderTarget(rend, tex);
        SDL_RenderCopy(rend, texImg, NULL, NULL);
        SDL_SetRenderTarget(rend, NULL);
    
    } catch(...) {
        tex = SDL_CreateTexture(rend, Main::getWindowPixelFormat(), SDL_TEXTUREACCESS_TARGET, 100, 100);
        if(tex==nullptr) throw std::exception();
    }

    //Set canvas parameters
    pos = {10, 10};
    SDL_QueryTexture(tex, NULL, NULL, &dims.x, &dims.y);
}
Canvas::~Canvas() {
    SDL_DestroyTexture(tex);
}

void Canvas::tick()
{
    //Save using CTRL+S
    if(Input::isModKeyDown(KMOD_CTRL) && nch::Input::keyDownTime(SDLK_s)==1) {
        printf("Saving...\n");
    }
    //Scale using scroll wheel
    int mwd = Input::getMouseWheelDelta();
    if(mwd!=0) {
        Vec2i cPosLast = getCursorPos();
        
        scale += mwd;
        if(scale<1) scale = 1;
        
        Vec2i cPosCurr = getCursorPos();
        Vec2i cPosDelta = cPosCurr-cPosLast;
        
        Log::log("(%d, %d)", cPosDelta.x, cPosDelta.y);
        pos += (cPosDelta*scale);
        // pos -= delta;
        //pos += (currMousePos*((double)lastScale/scale)-currMousePos);
    }
    //Translate using middle mouse click
    if(Input::mouseDownTime(2)==1) {
        grabbedMousePos = { Input::getMouseX(), Input::getMouseY() };
        grabbedPos = pos;
    }
    if(Input::isMouseDown(2)) {
        Vec2i currMousePos = { Input::getMouseX(), Input::getMouseY() };
        pos = grabbedPos-(grabbedMousePos-currMousePos);
    } else {
        grabbedMousePos = {-1234567, -1234567};
        grabbedPos = {-1234567, -1234567};
    }
}
void Canvas::drawCopy()
{
    SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
    Rect canvasDst(workspace.x1()+pos.x, workspace.y1()+pos.y, dims.x*scale, dims.y*scale);
    SDL_RenderCopy(rend, tex, NULL, &canvasDst.r);
}

Vec2i Canvas::getCursorPos()
{
    return Vec2i(
        std::floor((Input::getMouseX()-pos.x-workspace.x1())/(double)scale),
        std::floor((Input::getMouseY()-pos.y-workspace.y1())/(double)scale)
    );
}
FRect Canvas::getCursorSquare()
{
    auto cursorPos = getCursorPos();
    return FRect(
        workspace.x1()+pos.x+cursorPos.x*scale,
        workspace.y1()+pos.y+cursorPos.y*scale,
        scale, scale
    );
}

Rect Canvas::getWorkspace() {
    return workspace;
}
Vec2i Canvas::getPos() {
    return pos;
}
Vec2i Canvas::getDims() {
    return dims;
}
double Canvas::getScale() {
    return scale;
}
void Canvas::updateWorkspace(nch::Rect wsRect) {
    workspace = wsRect;
}

void Canvas::drawPixel(const nch::Vec2i& pos, const nch::Color& col)
{
    auto oldTgt = SDL_GetRenderTarget(rend); {
        SDL_SetRenderTarget(rend, tex);
        SDL_SetRenderDrawColor(rend, col.r, col.g, col.b, col.a);
        SDL_RenderDrawPoint(rend, pos.x, pos.y);
    } SDL_SetRenderTarget(rend, oldTgt);
}
void Canvas::drawLine(const nch::Vec2i& pos0, const nch::Vec2i& pos1, const nch::Color& col)
{
    auto oldTgt = SDL_GetRenderTarget(rend); {
        SDL_SetRenderTarget(rend, tex);
        SDL_SetRenderDrawColor(rend, col.r, col.g, col.b, col.a);
        SDL_RenderDrawLine(rend, pos0.x, pos0.y, pos1.x, pos1.y);
    } SDL_SetRenderTarget(rend, oldTgt);
}