#include "Paint.h"
#include <SDL2/SDL_image.h>
#include <cmath>
#include <nch/cpp-utils/fs-utils.h>
#include <nch/cpp-utils/log.h>
#include <nch/cpp-utils/timer.h>
#include <nch/rmlui-utils/rml-utils.h>
#include <nch/sdl-utils/input.h>
#include <nch/sdl-utils/texture-utils.h>
#include <sstream>
#include "Canvas.h"
#include "GUIs.h"
#include "Main.h"
#include "Types.h"
using namespace nch;

Paint::Paint(SDL_Renderer* rend, std::string openedImgPath)
{
    webview.initContext("paint");
    webview.rmlLoadDocumentAsset("paint.html");
    reload();

    //Load image onto canvas
    canv = new Canvas(rend, openedImgPath);
}
Paint::~Paint() {
    delete canv;
}

void Paint::tick()
{
    //Update workspace location
    auto doc = webview.getWorkingDocument();
    FRect toolbarRect = RmlUtils::getElementBox(doc->GetElementById("toolbar"));
    int tbx2 = toolbarRect.x2();
    if(toolbarRect.r.w==-1) tbx2 = 0;
    canv->updateWorkspace(Rect(tbx2, 0, Main::getWidth()-tbx2, Main::getHeight()));

    //Tick webview/process reloads and events
    webview.tick();

    if(Input::keyDownTime(SDLK_F5)==1) {
        reload();
    }
    bool justSaved = false;
    if(Input::isModKeyDown(KMOD_CTRL) && nch::Input::keyDownTime(SDLK_s)==1) {
        Timer tim("saving image file to PNG", true);
        canv->save();
        justSaved = true;
    }
    if(!justSaved) {
        if(Input::keyDownTime(SDLK_s)==1) { selectTool(SELECTION); }
        if(Input::keyDownTime(SDLK_e)==1) { selectTool(ERASER); }
        if(Input::keyDownTime(SDLK_d)==1) { selectTool(PENCIL); }
        if(Input::keyDownTime(SDLK_f)==1) { selectTool(FILL_BUCKET); }
    }

    {
        //Change color or open color picker
        Rml::ElementList eColorSquares;
        doc->GetElementsByClassName(eColorSquares, "color-square");
        for(int i = 0; i<eColorSquares.size(); i++) {
            FRect csRect = RmlUtils::getElementBox(eColorSquares[i]);
            if(csRect.contains(Input::getMouseX(), Input::getMouseY())) {
                if(Input::mouseDownTime(1)==1) {
                    toolColor = selectColorSquare(eColorSquares[i]->GetId());
                }
                if(Input::mouseDownTime(3)==1) {
                    GUIs::addColorPickerDialog(toolColor);
                }
            }
        }
        //Change tool
        Rml::ElementList eToolSquares;
        doc->GetElementsByClassName(eToolSquares, "tool-square");
        for(int i = 0; i<eToolSquares.size(); i++) {
            FRect csRect = RmlUtils::getElementBox(eToolSquares[i]);
            if(csRect.contains(Input::getMouseX(), Input::getMouseY())) {
                if(Input::mouseDownTime(1)==1) {
                    selectTool(i);
                }
            }
        }
    }

    canv->tick();
}

void Paint::draw(SDL_Renderer* rend)
{
    //Set cursor location
    lastCursorPos = cursorPos;
    cursorPos = canv->getCursorPos();
    Rect workspace = canv->getWorkspace();

    //Draw when holding mouse down
    if(Input::isMouseDown(1)) {
        switch (toolType) {
            case PENCIL: { canv->drawLine(lastCursorPos, cursorPos, toolColor); } break;
            case ERASER: { canv->drawLine(lastCursorPos, cursorPos, Color(255, 255, 255, 0)); } break;
            case FILL_BUCKET: { canv->floodPixels(cursorPos, toolColor); } break;
        }
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

        //Outline color (oscillates between light gray and white)
        int oc; {
            uint64_t ms = Timer::getTicks();
            double angle = 3.14159265358979323846*(ms/500.0);
            oc = static_cast<int>(std::lround(191.0+64.0*std::sin(angle)));
            if(oc<127) oc = 127;
            if(oc>255) oc = 255;
        }

        //Draw canvas outline
        SDL_SetRenderDrawColor(rend, oc, oc, oc, 255);
        auto rCanvOutline = canv->getDst();
        rCanvOutline.r.x--;     rCanvOutline.r.y--;
        rCanvOutline.r.w+=2;    rCanvOutline.r.h+=2;
        SDL_RenderDrawRectF(rend, &rCanvOutline.r);
        //Draw canvas
        canv->drawCopy();
        //Draw cursor pixel
        SDL_SetRenderDrawColor(rend, oc, oc, oc, 255);
        FRect cursorDst = canv->getCursorSquare();
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
    toolColor = selectColorSquare(0);
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

void Paint::selectTool(int toolType)
{
    auto doc = webview.getWorkingDocument();
    Rml::ElementList eColorSquares;
    doc->GetElementsByClassName(eColorSquares, "tool-square");
    for(int i = 0; i<eColorSquares.size(); i++) {
        if(i==toolType) {
            Paint::toolType = toolType;
            eColorSquares[i]->SetClassNames("tool-square selected");
        } else {
            eColorSquares[i]->SetClassNames("tool-square");
        }
    }
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