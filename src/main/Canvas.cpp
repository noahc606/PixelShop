#include "Canvas.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <cstddef>
#include <nch/cpp-utils/filepath.h>
#include <nch/cpp-utils/fs-utils.h>
#include <nch/cpp-utils/log.h>
#include <nch/sdl-utils/input.h>
#include <nch/sdl-utils/rect.h>
#include <nch/sdl-utils/texture-utils.h>
#include "Main.h"
using namespace nch;

Canvas::Canvas(SDL_Renderer* rend, const std::string& openedImgPath)
{
    Canvas::rend = rend;

    try {
        if(openedImgPath=="") throw std::exception();
        if(!FsUtils::fileExists(openedImgPath)) throw std::exception();

        //Populate file name and directory
        {
            FilePath fp(openedImgPath);
            fileName = fp.getObjectName();
            fileDir = fp.getParentDirPath();
            Log::log("Opened image with name \"%s\" within directory \"%s\".", fileName.c_str(), fileDir.c_str());
        }
        
        //Create surface 'surf' from loading the image @ the provided 'path'.
        SDL_Surface* surfRaw = IMG_Load(openedImgPath.c_str());
        surf = SDL_ConvertSurface(surfRaw, Main::getPixelFormat(), 0);  SDL_FreeSurface(surfRaw);
        if(surf==nullptr) throw std::exception();
    } catch(...) {
        //Create empty surface 'surf' with a predetermined
        surf = SDL_CreateRGBSurfaceWithFormat(0, 100, 100, 32, SDL_PIXELFORMAT_RGBA8888);
        if(surf==nullptr) throw std::exception();
    }

    //Create texture 'tex' with same size as 'surf'
    tex = SDL_CreateTexture(rend, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, surf->w, surf->h);
    if(tex==nullptr) throw std::exception();

    //Set canvas parameters
    pos = {10, 10};
    SDL_QueryTexture(tex, NULL, NULL, &dims.x, &dims.y);
}
Canvas::~Canvas() {
    SDL_DestroyTexture(tex);
}

void Canvas::tick()
{
    //Scale using scroll wheel
    int mwd = Input::getMouseWheelDelta();
    if(mwd!=0) {
        Vec2i cPosLast = getCursorPos();
        
        scale += mwd;
        if(scale<1) scale = 1;
        
        Vec2i cPosCurr = getCursorPos();
        Vec2i cPosDelta = cPosCurr-cPosLast;
        
        pos += (cPosDelta*scale);
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

    FRect canvasDst = getDst();
    Rect cr = Rect(canvasDst.r.x, canvasDst.r.y, canvasDst.r.w, canvasDst.r.h);

    SDL_UpdateTexture(tex, NULL, surf->pixels, surf->w*4);
    SDL_RenderCopy(rend, tex, NULL, &cr.r);
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

FRect Canvas::getDst() {
    return FRect(workspace.x1()+pos.x, workspace.y1()+pos.y, dims.x*scale, dims.y*scale);
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
Color Canvas::getPixel(const nch::Vec2i& pos)
{
    return TexUtils::getPixelColor(surf, pos.x, pos.y);
}

void Canvas::saveAs(std::string savedFileName)
{
    if(!FsUtils::dirExists(fileDir)) {
        Log::warnv(__PRETTY_FUNCTION__, "Parent directory \"%s\" of save location doesn't seem to exist", fileDir.c_str());
        return;
    }

    IMG_SavePNG(surf, (fileDir+"/"+savedFileName).c_str());
}
void Canvas::save()
{
    if(saveNeedsUniqueName) {
        saveAs(fileName+"_"+Main::getTimeFmtted());
        return;
    }
    saveAs(fileName);
}

void Canvas::updateWorkspace(nch::Rect wsRect) {
    workspace = wsRect;
}

void Canvas::drawPixel(const nch::Vec2i& pos, const nch::Color& col)
{
    if(pos.x<0 || pos.y<0 || pos.x>=surf->w || pos.y>=surf->h) return;
    TexUtils::setPixelColor(surf, pos.x, pos.y, col.getRGBA());
}

void Canvas::drawLine(const nch::Vec2i& pos0, const nch::Vec2i& pos1, const nch::Color& col)
{
    Vec2i p0 = pos0;
    Vec2i p1 = pos1;

    int dx = std::abs(p1.x - p0.x);
    int dy = std::abs(p1.y - p0.y);

    int sx = (p0.x < p1.x) ? 1 : -1;
    int sy = (p0.y < p1.y) ? 1 : -1;

    int err = dx - dy;

    int x = p0.x;
    int y = p0.y;

    while (true) {
        drawPixel({x, y}, col);

        if (x == p1.x && y == p1.y) break;

        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
}
void Canvas::floodPixels(const nch::Vec2i& pos, const nch::Color& col)
{
    if(pos.x<0 || pos.y<0 || pos.x>=surf->w || pos.y>=surf->h) return;
    Rect surfRect = Rect(0, 0, surf->w-1, surf->h-1);
    std::set<std::pair<int, int>> traversed;
    auto lastCol = TexUtils::getPixelColor(surf->pixels, surf->format, surf->pitch, pos.x, pos.y);
    floodPixelsHelper(surfRect, traversed, pos, col, lastCol);
}

void Canvas::floodPixelsHelper(const nch::Rect& surfRect, std::set<std::pair<int, int>>& traversed, const nch::Vec2i& pos, const nch::Color& newCol, const nch::Color& lastCol) {
    if(traversed.find({pos.x, pos.y})!=traversed.end()) { return; }
    traversed.insert({pos.x, pos.y});
    if(!surfRect.contains(pos.x, pos.y)) { return; }
    
    auto pxCol = TexUtils::getPixelColor(surf->pixels, surf->format, surf->pitch, pos.x, pos.y);
    if(pxCol.getRGB()!=lastCol.getRGB()) { return; }

    TexUtils::setPixelColor(surf, pos.x, pos.y, newCol.getRGBA());

    floodPixelsHelper(surfRect, traversed, pos+Vec2i( 0,-1), newCol, lastCol);
    floodPixelsHelper(surfRect, traversed, pos+Vec2i( 0, 1), newCol, lastCol);
    floodPixelsHelper(surfRect, traversed, pos+Vec2i(-1, 0), newCol, lastCol);
    floodPixelsHelper(surfRect, traversed, pos+Vec2i( 1, 0), newCol, lastCol);
}