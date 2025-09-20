#pragma once
#include <SDL2/SDL.h>
#include <nch/cpp-utils/color.h>
#include <nch/math-utils/vec2.h>
#include <nch/sdl-utils/rect.h>
#include <set>

class Canvas {
public:
    Canvas(SDL_Renderer* rend, const std::string& openedImgPath);
    ~Canvas();
    void tick();
    void drawCopy();

    nch::Vec2i getCursorPos();
    nch::FRect getCursorSquare();
    nch::FRect getDst();
    nch::Rect getWorkspace();
    nch::Vec2i getPos(); nch::Vec2i getDims(); double getScale();
    
    void saveAs(std::string savedFileName);
    void save();
    void updateWorkspace(nch::Rect wsRect);
    void drawPixel(const nch::Vec2i& pos, const nch::Color& col);
    void drawLine(const nch::Vec2i& pos0, const nch::Vec2i& pos1, const nch::Color& col);
    void floodPixels(const nch::Vec2i& pos, const nch::Color& col);
private:

    void floodPixelsHelper(const nch::Rect& surfRect, std::set<std::pair<int, int>>& traversed, const nch::Vec2i& pos, const nch::Color& newCol, const nch::Color& lastCol);

    SDL_Renderer* rend = nullptr;
    SDL_Surface* surf = nullptr;
    SDL_Texture* tex = nullptr;

    std::string fileDir = "";
    std::string fileName = "";
    bool saveNeedsUniqueName = true;

    nch::Rect workspace = nch::Rect(0, 0, 1, 1);
    nch::Vec2i grabbedMousePos = {-1, -1}, grabbedPos = {-1, -1};
    nch::Vec2i pos = {0, 0}, dims = {1, 1};
    int scale = 5.0;
};