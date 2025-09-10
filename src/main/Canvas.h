#pragma once
#include <SDL2/SDL.h>
#include <nch/cpp-utils/color.h>
#include <nch/math-utils/vec2.h>
#include <nch/sdl-utils/rect.h>

class Canvas {
public:
    Canvas(SDL_Renderer* rend, const std::string& path);
    ~Canvas();
    void tick();
    void drawCopy();

    nch::Vec2i getCursorPos();
    nch::FRect getCursorSquare();

    nch::Rect getWorkspace();
    nch::Vec2i getPos();
    nch::Vec2i getDims();
    double getScale();
    void updateWorkspace(nch::Rect wsRect);
    void drawPixel(const nch::Vec2i& pos, const nch::Color& col);
    void drawLine(const nch::Vec2i& pos0, const nch::Vec2i& pos1, const nch::Color& col);
private:
    SDL_Renderer* rend = nullptr;
    SDL_Texture* tex = nullptr;

    nch::Rect workspace = nch::Rect(0, 0, 1, 1);
    nch::Vec2i grabbedMousePos = {-1, -1}, grabbedPos = {-1, -1};
    nch::Vec2i pos = {0, 0}, dims = {1, 1};
    int scale = 5.0;
};