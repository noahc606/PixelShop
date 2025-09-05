#include "Paint.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <exception>
#include <nch/cpp-utils/fs-utils.h>
#include <nch/cpp-utils/log.h>
#include <nch/cpp-utils/timer.h>
#include <nch/rmlui-utils/rml-utils.h>
#include <nch/sdl-utils/input.h>
#include <nch/sdl-utils/texture-utils.h>
#include <sstream>
#include "GUIs.h"
#include "Main.h"
#include "Types.h"
using namespace nch;

Paint::Paint(SDL_Renderer* rend, std::string path)
{
    webview.initContext("paint");
    webview.rmlLoadDocumentAsset("paint.html");
    reload();

    //Load image onto canvas
    try {
        if(path=="") throw std::exception();
        if(!FsUtils::fileExists(path)) throw std::exception();
        
        //Create surface 'surfImg' from loading the image @ the provided 'path'.
        SDL_Surface* surfImg = IMG_Load(path.c_str());
        if(surfImg==nullptr) throw std::exception();

        //Create empty 'canvas' with same size as 'surfImg'
        canvas = SDL_CreateTexture(rend, Main::getWindowPixelFormat(), SDL_TEXTUREACCESS_TARGET, surfImg->w, surfImg->h);
        if(canvas==nullptr) throw std::exception();
        TexUtils::clearTexture(rend, canvas);

        //Create texture 'texImg' from 'surfImg'
        SDL_Texture* texImg = SDL_CreateTextureFromSurface(rend, surfImg);
        SDL_FreeSurface(surfImg);
        if(texImg==nullptr) throw std::exception();
        //Copy loaded 'texImg' to canvas
        SDL_SetRenderTarget(rend, canvas);
        SDL_RenderCopy(rend, texImg, NULL, NULL);
        SDL_SetRenderTarget(rend, NULL);
    
    } catch(...) {
        canvas = SDL_CreateTexture(rend, Main::getWindowPixelFormat(), SDL_TEXTUREACCESS_TARGET, 100, 100);
        if(canvas==nullptr) throw std::exception();
    }

    //Set canvas parameters
    cvsPos = {10, 10};
    SDL_QueryTexture(canvas, NULL, NULL, &cvsDims.x, &cvsDims.y);
}
Paint::~Paint() {
    SDL_DestroyTexture(canvas);
}

void Paint::tick()
{
    //Update workspace location
    auto doc = webview.getWorkingDocument();
    FRect toolbarRect = RmlUtils::getElementBox(doc->GetElementById("toolbar"));
    int tbx2 = toolbarRect.x2();
    if(toolbarRect.r.w==-1) tbx2 = 0;
    workspace = Rect(tbx2, 0, Main::getWidth()-tbx2, Main::getHeight());

    //Tick webview/process reloads and events
    webview.tick();
    if(Input::keyDownTime(SDLK_F5)==1) {
        reload();
    }
    //Color square clicking
    {
        Rml::ElementList eColorSquares;
        doc->GetElementsByClassName(eColorSquares, "color-square");
        for(int i = 0; i<eColorSquares.size(); i++) {
            FRect csRect = RmlUtils::getElementBox(eColorSquares[i]);
            if(csRect.contains(Input::getMouseX(), Input::getMouseY())) {
                if(Input::mouseDownTime(1)==1) {
                    brushColor = selectColorSquare(eColorSquares[i]->GetId());
                }
                if(Input::mouseDownTime(3)==1) {
                    GUIs::addColorPickerDialog(brushColor);
                }
            }
        }
    }



    //Key inputs
    if(Input::isModKeyDown(KMOD_CTRL) && nch::Input::keyDownTime(SDLK_s)==1) {
        printf("Saving...\n");
    }
}

void Paint::draw(SDL_Renderer* rend)
{
    //Set mouse location
    int mx = (Input::getMouseX()-cvsPos.x-workspace.x1())/cvsZoom;
    int my = (Input::getMouseY()-cvsPos.y-workspace.y1())/cvsZoom;
    lastCursorPos = cursorPos;
    cursorPos = {mx, my};
    
    //Draw when holding mouse down
    if(nch::Input::isMouseDown(1) || nch::Input::isMouseDown(3)) {
        SDL_SetRenderTarget(rend, canvas);
        drawPixel(rend, cursorPos);
        SDL_SetRenderDrawColor(rend, brushColor.r, brushColor.g, brushColor.b, 255);
        SDL_RenderDrawLine(rend, lastCursorPos.x, lastCursorPos.y, cursorPos.x, cursorPos.y);  
        SDL_SetRenderTarget(rend, NULL);
    }

    /* Draw workspace */
    {
        //Transparent background
        int bgSquareSize = 32;
        for(int ix = 0; ix<=workspace.r.w/bgSquareSize; ix++) {
        for(int iy = 0; iy<=workspace.r.h/bgSquareSize; iy++) {
            if((ix+iy)%2==0) { SDL_SetRenderDrawColor(rend, 95, 95, 95, 255); }
            else             { SDL_SetRenderDrawColor(rend, 127, 127, 127, 255); }
            
            Rect dst(workspace.x1()+ix*bgSquareSize, workspace.y1()+iy*bgSquareSize, bgSquareSize, bgSquareSize);
            SDL_RenderFillRect(rend, &dst.r);
        }}
        
        //Draw canvas
        SDL_SetTextureBlendMode(canvas, SDL_BLENDMODE_BLEND);
        Rect canvasDst(workspace.x1()+cvsPos.x, workspace.y1()+cvsPos.y, cvsDims.x*cvsZoom, cvsDims.y*cvsZoom);
        SDL_RenderCopy(rend, canvas, NULL, &canvasDst.r);

        //Draw cursor pixel
        SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
        FRect cursorDst(workspace.x1()+cvsPos.x+cursorPos.x*cvsZoom, workspace.y1()+cvsPos.y+cursorPos.y*cvsZoom, cvsZoom, cvsZoom);
        SDL_RenderDrawRectF(rend, &cursorDst.r);
    }

    //Draw toolbar
    webview.resize({Main::getWidth(), Main::getHeight()});
    webview.render();
    webview.drawCopy({0, 0});
}

void Paint::reload()
{
    webview.reload();

    auto doc = webview.getWorkingDocument();
    Rml::ElementList eColorSquares;
    doc->GetElementsByClassName(eColorSquares, "color-square");
    for(int i = 0; i<eColorSquares.size(); i++) {
        std::stringstream id; id << "color-square-"  << i;
        eColorSquares[i]->SetId(id.str());
    }

    //Set selected colors
    setColorSquare(0, Color(255, 0, 0));
    setColorSquare(1, Color(0, 255, 0));
    setColorSquare(2, Color(0, 0, 255));
    setColorSquare(3, Color(255, 255, 255));
    setColorSquare(4, Color(0, 0, 0));
}

void Paint::drawPixel(SDL_Renderer* rend, nch::Vec2i pos)
{
    SDL_SetRenderDrawColor(rend, brushColor.r, brushColor.g, brushColor.b, 255);
    SDL_RenderDrawPoint(rend, pos.x, pos.y);
}

void Paint::setColorSquare(int idNo, nch::Color col)
{
    std::stringstream idStr; idStr << "color-square-" << idNo;
    auto doc = webview.getWorkingDocument();
    auto elem = doc->GetElementById(idStr.str());

    std::stringstream style;
    style << "background-color: " << col.toStringB16(true) << ";";
    elem->SetAttribute("style", style.str());
}

Color Paint::selectColorSquare(std::string id)
{
    Color ret;

    auto doc = webview.getWorkingDocument();
    Rml::ElementList eColorSquares;
    doc->GetElementsByClassName(eColorSquares, "color-square");
    for(int i = 0; i<eColorSquares.size(); i++) {
        if(eColorSquares[i]->GetId()==id) {
            std::string styleVal = RmlUtils::getElementAttributeValue(eColorSquares[i], "style");
            styleVal = styleVal.substr(styleVal.find("#"), 9);
            ret.setFromB16Str(styleVal);
            eColorSquares[i]->SetClassNames("color-square selected");
        } else {
            eColorSquares[i]->SetClassNames("color-square");
        }
    }

    return ret;
}
Color Paint::selectColorSquare(int idNo)
{
    std::stringstream idStr; idStr << "color-square-" << idNo;
    return selectColorSquare(idStr.str());
}