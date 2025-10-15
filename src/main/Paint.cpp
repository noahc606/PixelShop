#include "Paint.h"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_rect.h>
#include <cmath>
#include <nch/cpp-utils/fs-utils.h>
#include <nch/cpp-utils/log.h>
#include <nch/cpp-utils/string-utils.h>
#include <nch/cpp-utils/timer.h>
#include <nch/rmlui-utils/rml-utils.h>
#include <nch/sdl-utils/input.h>
#include <nch/sdl-utils/texture-utils.h>
#include <sstream>
#include <stdexcept>
#include <vector>
#include "Canvas.h"
#include "ContextMenu.h"
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

void Paint::reload()
{
    webview.reload();
    
    auto doc = webview.getWorkingDocument();
    webEvents.trackEvent(WebEvent::MOUSE_DOWN, doc->GetElementById("menupanel-file"));
    webEvents.trackEvent(WebEvent::MOUSE_DOWN, doc->GetElementById("menupanel-edit"));
    webEvents.trackEvent(WebEvent::MOUSE_DOWN, doc->GetElementById("tool-color-picker"));
    
    Rml::ElementList eColorSquares;
    doc->GetElementsByClassName(eColorSquares, "color-square");
    for(int i = 0; i<eColorSquares.size(); i++) {
        std::stringstream id; id << "color-square-"  << i;
        eColorSquares[i]->SetId(id.str());
        setColorSquare(i, Color(0, 0, 0));
    }

    std::vector<Color> colors = {
        Color(0, 0, 0),
        Color(255, 255, 255),
        Color(255, 0, 0),
        Color(0, 255, 0),
        Color(0, 0, 255),
        Color(255, 255, 0),
        Color(255, 0, 255),
        Color(0, 255, 255),
        Color(255, 127, 0),
        Color(127, 0, 127),
        Color(0, 0, 127),
        Color(0, 127, 0)
    };
    for(int i = 0; i<colors.size(); i++) {
        setColorSquare(i, colors[i]);
    }
    toolColor = selectColorSquare(0);
}



void Paint::tick()
{
    //Update workspace state
    updateWorkspace();

    //Tick webview/process reloads and events
    webview.tick();
    WebEvent wevt;
    while((wevt = webEvents.popEvent()).exists()) {
        FRect elemRect = RmlUtils::getElementBox(wevt.getElement());

        if(wevt.getElementID()=="tool-color-picker") {
            selectTool(-1);
            selectColorPicker(true);
        }

        if(wevt.getElementID()=="menupanel-file") {
            if(GUIs::doesContextMenuExist()) {
                GUIs::removeContextMenus();
            } else {
                GUIs::setContextMenu(Vec2i(elemRect.x1(), elemRect.y2()+2), ContextMenu::FILE);
            }
        }
        if(wevt.getElementID()=="menupanel-edit") {
            if(GUIs::doesContextMenuExist()) {
                GUIs::removeContextMenus();
            } else {
                GUIs::setContextMenu(Vec2i(elemRect.x1(), elemRect.y2()+2), ContextMenu::EDIT);
            }
        }
    }

    //Events emanating from GUIs
    {
        if(GUIs::tryPopPendingEvent("new_image")) {
            //canv->promptNewImage();
        }
        if(GUIs::tryPopPendingEvent("open_image")) {
            //canv->promptOpenImage();
        }
        if(GUIs::tryPopPendingEvent("save")) {
            canv->save();
            GUIs::removeContextMenus();
        }
        if(GUIs::tryPopPendingEvent("save_as")) {
            //canv->promptSaveAs();
        }

        if(GUIs::tryPopPendingEvent("flatten_image")) {
            canv->editFlattenImage();
            GUIs::removeContextMenus();
        }
        if(GUIs::tryPopPendingEvent("flatten_image_translucency")) {
            Log::log("Flattening image translucency...");
            canv->editFlattenImageTranslucency();
            GUIs::removeContextMenus();
        }
        if(GUIs::tryPopPendingEvent("resize_image")) {
            GUIs::addGenericPopup("window/popup/resize.html");
            GUIs::removeContextMenus();
        }
    }

    if(focusedWorkspace) {
        tickingControls();
    }
}
void Paint::draw(SDL_Renderer* rend)
{
    //Set cursor location
    lastCursorPos = cursorPos;
    cursorPos = canv->getCursorPos();
    Rect workspace = canv->getWorkspace();
    //Color picker release
    if(!colorPickerReleased && !Input::isMouseDown(1)) {
        colorPickerReleased = true;
    }

    //Draw when holding mouse down
    if(focusedWorkspace && Input::isMouseDown(1)) {
        if(workspace.contains(Input::getMouseX(), Input::getMouseY())) {
            drawingLeftMouseDown();
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
        //Draw selection outline
        SDL_SetRenderDrawColor(rend, oc, oc, oc/4, 255);
        FRect rSelOutline = canv->getScreenSquare(selection.r.x, selection.r.y, selection.r.w, selection.r.h);
        SDL_RenderDrawRectF(rend, &rSelOutline.r);
    }

    //Draw sidepanel
    webview.resize({Main::getWidth(), Main::getHeight()});
    webview.render();
    webview.drawCopy({0, 0});

    //Draw out-of-focus overlay if out-of-focus
    if(!focusedWorkspace) {
        int alpha = ((int)workspaceUnfocusedAge)*12;
        if(alpha<0) alpha = 0;
        if(alpha>127) alpha = 127;
        SDL_SetRenderDrawColor(rend, 0, 0, 0, alpha);
        SDL_SetRenderDrawBlendMode(rend, SDL_BLENDMODE_BLEND);
        SDL_RenderFillRect(rend, NULL);
    }
}
void Paint::drawingLeftMouseDown()
{
    if(!colorPickerSelected && colorPickerReleased) {
        switch(toolType) {
            case SELECTION: {
                auto cursorPos2 = cursorPos;
                if(cursorPos2.x<0) cursorPos2.x = 0;
                if(cursorPos2.y<0) cursorPos2.y = 0;
                if(cursorPos2.x>=canv->getDims().x) cursorPos2.x = canv->getDims().x-1;
                if(cursorPos2.y>=canv->getDims().y) cursorPos2.y = canv->getDims().y-1;
                
                if(Input::mouseDownTime(1)==1) {
                    selection.r.x = cursorPos2.x;
                    selection.r.y = cursorPos2.y;
                    selection.r.w = 1;
                    selection.r.h = 1;
                } else {
                    selection.r.w = cursorPos2.x-selection.r.x;
                    selection.r.h = cursorPos2.y-selection.r.y;
                    if(selection.r.w>=0) selection.r.w++;
                    if(selection.r.h>=0) selection.r.h++;

                    if(selection.x2()>canv->getDims().x) {
                        selection.r.w = canv->getDims().x-selection.r.x;
                    }
                    if(selection.y2()>canv->getDims().y) {
                        selection.r.h = canv->getDims().y-selection.r.y;
                    }
                }
            } break;
            case PENCIL:      { canv->editDrawLine(lastCursorPos, cursorPos, toolColor); } break;
            case ERASER:      { canv->editDrawLine(lastCursorPos, cursorPos, Color(255, 255, 255, 0)); } break;
            case FILL_BUCKET: { canv->editFloodPixels(cursorPos, toolColor); } break;
        }

        return;
    }
    
    if(colorPickerReleased) {
        try {
            auto pxCol = canv->getPixel(cursorPos);
            
            selectColorPicker(false);
            selectTool(toolType);
            setColorSquare(lastColorSquareIdSelected, pxCol);
            toolColor = selectColorSquare(lastColorSquareIdSelected);
        } catch(...) {}
    }
    colorPickerReleased = false;
}
void Paint::tickingControls()
{
    auto doc = webview.getWorkingDocument();

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
                    GUIs::addColorPickerPopup(toolColor);
                }
            }
        }
        //Change tool
        Rml::ElementList eToolSquares;
        doc->GetElementsByClassName(eToolSquares, "tool-square");
        for(int i = 0; i<eToolSquares.size(); i++) {
            if(!eToolSquares[i]->IsClassSet("toolbar-primary")) continue;

            FRect fr = RmlUtils::getElementBox(eToolSquares[i]);
            if(fr.contains(Input::getMouseX(), Input::getMouseY())) {
                if(Input::mouseDownTime(1)==1) {
                    selectTool(i);
                    selectColorPicker(false);
                }
            }
        }
    }

    canv->tick();
}
void Paint::updateWorkspace()
{
    //Update canvas workspace size
    auto doc = webview.getWorkingDocument();
    FRect sidepanelRect = RmlUtils::getElementBox(doc->GetElementById("sidepanel"));
    int tbx2 = sidepanelRect.x2();
    if(sidepanelRect.r.w==-1) tbx2 = 0;
    canv->updateWorkspace(Rect(tbx2, 0, Main::getWidth()-tbx2, Main::getHeight()));

    //Update workspace focus state
    bool wsFocus = (
        GUIs::doesContextMenuExist() ||
        GUIs::doesWindowTypeExist(Window::POPUP_COLOR_PICKER) ||
        GUIs::doesWindowTypeExist(Window::POPUP_GENERIC)
    );
    if(wsFocus) { focusedWorkspace = false; }
    else        { focusedWorkspace = true; 
    }
    //Update workspace-unfocused age
    if(!focusedWorkspace) { workspaceUnfocusedAge++; }
    else                  { workspaceUnfocusedAge = 0; }
}


void Paint::setColorSquare(int idNo, nch::Color col)
{
    std::stringstream idStr; idStr << "color-square-" << idNo;
    auto doc = webview.getWorkingDocument();
    auto elem = doc->GetElementById(idStr.str());
    if(elem==nullptr) return;

    std::stringstream style;
    style << "background-color: " << col.toStringB16(true) << ";";
    elem->SetAttribute("style", style.str());
}

void Paint::selectTool(int toolType)
{
    auto doc = webview.getWorkingDocument();
    Rml::ElementList eToolbarItems;
    doc->GetElementsByClassName(eToolbarItems, "toolbar-primary");
    for(int i = 0; i<eToolbarItems.size(); i++) {
        if(i==toolType) {
            Paint::toolType = toolType;
            eToolbarItems[i]->SetClassNames("toolbar-primary tool-square selected");
        } else {
            eToolbarItems[i]->SetClassNames("toolbar-primary tool-square");
        }
    }


}

void Paint::selectColorPicker(bool selected)
{
    colorPickerSelected = selected;
    auto doc = webview.getWorkingDocument();
    Rml::Element* eToolColorPicker = doc->GetElementById("tool-color-picker");
    if(eToolColorPicker!=nullptr) {
        if(selected) { eToolColorPicker->SetClassNames("toolbar-secondary tool-square selected"); }
        else         { eToolColorPicker->SetClassNames("toolbar-secondary tool-square"); }
    }
}

Color Paint::selectColorSquare(std::string id)
{
    bool fail = true;
    if(StringUtils::aHasPrefixB(id, "color-square-")) {
        int idNo = -1;
        try { idNo = std::stoi(id.substr(std::string("color-square-").size())); } catch(...) {}
        if(idNo!=-1) {
            fail = false;
            lastColorSquareIdSelected = idNo;
        }
    }
    if(fail) {
        Log::error(__PRETTY_FUNCTION__, "Invalid ID \"%s\"", id.c_str());
        throw std::out_of_range("");
    }

    Color ret;

    auto doc = webview.getWorkingDocument();
    Rml::ElementList eToolbarItems;
    doc->GetElementsByClassName(eToolbarItems, "toolbar-palette");
    for(int i = 0; i<eToolbarItems.size(); i++) { try {
        if(eToolbarItems[i]->GetId()==id) {
            std::string styleVal = RmlUtils::getElementAttributeValue(eToolbarItems[i], "style");
            styleVal = styleVal.substr(styleVal.find("#"), 9);
            ret.setFromB16Str(styleVal);
            eToolbarItems[i]->SetClassNames("toolbar-palette color-square selected");
        } else {
            eToolbarItems[i]->SetClassNames("toolbar-palette color-square");
        }
    } catch(...) {}}

    return ret;
}
Color Paint::selectColorSquare(int idNo)
{
    std::stringstream idStr; idStr << "color-square-" << idNo;
    return selectColorSquare(idStr.str());
}