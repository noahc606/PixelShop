#include "Paint.h"
#include <nch/sdl-utils/Input.h>
#include "Main.h"

void Paint::init(SDL_Renderer* rend)
{
    canvas = SDL_CreateTexture(rend, Main::getWindowPixelFormat(), SDL_TEXTUREACCESS_TARGET, 100, 100);
    
    SDL_SetRenderTarget(rend, canvas);
    SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_NONE);
    SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
    SDL_RenderFillRect(rend, NULL);

    SDL_SetRenderTarget(rend, NULL);

}

void Paint::tick()
{
    int mx = nch::Input::getMouseX();
    int my = nch::Input::getMouseY();

    canvasRect.x = 10;
    canvasRect.y = 10;
    canvasRect.w = 500;
    canvasRect.h = 500;
    SDL_Rect cr = canvasRect;

    //Set lastMouseP(X, Y) and mouseP(X, Y).
    double stretchW = cr.w/100; double stretchH = cr.h/100;
    lastMousePX = mousePX; lastMousePY = mousePY;
    mousePX = (mx-cr.x)/stretchW; mousePY = (my-cr.y)/stretchH;

    //Key inputs
    if( nch::Input::isModKeyDown(KMOD_CTRL) && nch::Input::keyDownTime(SDLK_s)==1 ) {
        
        printf("Saving...\n");
    }
}

void Paint::draw(SDL_Renderer* rend)
{
    //SDL_RenderFillRect(rend, &dst);
    SDL_SetTextureBlendMode(canvas, SDL_BLENDMODE_BLEND);
    SDL_RenderCopy(rend, canvas, NULL, &canvasRect);
    
    if(nch::Input::isMouseDown(1)) {
        SDL_SetRenderTarget(rend, canvas);
        
        if(lastMousePX<0 || lastMousePY<0) {
            drawPixel(rend, mousePX, mousePY);
        } else {
            SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
            SDL_RenderDrawLine(rend, lastMousePX, lastMousePY, mousePX, mousePY);  
        }

        //drawPixel(rend, mousePX, mousePY);
        SDL_SetRenderTarget(rend, NULL);
    }
}

void Paint::drawPixel(SDL_Renderer* rend, int px, int py)
{
    SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
    SDL_RenderDrawPoint(rend, mousePX, mousePY);
}