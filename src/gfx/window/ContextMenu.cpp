#include "ContextMenu.h"
#include "GUIs.h"
#include <nch/rmlui-utils/rml-utils.h>
#include <nch/sdl-utils/input.h>
using namespace nch;

std::vector<std::map<std::string, std::vector<std::string>>> ContextMenu::itemSets = {
    {
        //{"new_image", {"New Image"}},
        //{"open_image", {"Open Image..."}},
        {"save", {"Save", "Ctrl+S"}},
        //{"save_as", {"Save As...", "Ctrl+Shift+S"}}
    },
    {
        //{"edit_undo", {"Undo", "Ctrl+Z"}},
        //{"edit_redo", {"Redo", "Ctrl+Y"}},
        {"flatten_image", {"Flatten Image"}},
        {"flatten_image_translucency", {"Flatten Image Translucency"}},
        {"resize_image", {"Resize Image..."}},
    }
};

ContextMenu::ContextMenu(const nch::Vec2i& spawnPos, int itemSet)
{
    ContextMenu::spawnPos = spawnPos;
    ContextMenu::itemSet = itemSet;
    int itemCount = 1;
    try { itemCount = itemSets.at(itemSet).size(); } catch(...) {}
    super_init(Window::CONTEXT_MENU, {270, 24*itemCount});

    reload();
}
ContextMenu::~ContextMenu(){}

void ContextMenu::reload() {
    super_reload();

    std::stringstream rml;
    try {
        auto map = itemSets.at(itemSet);
        for(auto& elem: map) {
            if(elem.second.size()==1) addItemToRML(rml, elem.first, elem.second.at(0), "");
            if(elem.second.size()==2) addItemToRML(rml, elem.first, elem.second.at(0), elem.second.at(1));
        } 
    } catch(...) {}
    
    auto doc = webview.getWorkingDocument();
    try {
        Rml::ElementList mainTags;
        doc->GetElementsByTagName(mainTags, "main");
        mainTags.at(0)->SetInnerRML(rml.str());
    } catch(...) {}

    //Add events to all existing context menu "item"s
    Rml::ElementList cmItems;
    doc->GetElementsByClassName(cmItems, "item");
    for(int i = 0; i<cmItems.size(); i++) {
        webEvents.trackEvent(WebEvent::MOUSE_DOWN, cmItems.at(i));
    }
}
void ContextMenu::tick() {
    super_tick();

    if(Input::keyDownTime(SDLK_ESCAPE)==1) {
        close();
    }

    WebEvent wevt;
    while((wevt = webEvents.popEvent()).exists()) {
        GUIs::addPendingEvent(wevt.getElementID());
    }
}
void ContextMenu::draw()
{
    super_draw();
}

Vec2i ContextMenu::getSpawnPos() {
    return spawnPos;
}

void ContextMenu::addItemToRML(std::stringstream& rml, std::string id, std::string itemDesc, std::string shortcutDesc)
{
    if(shortcutDesc=="") {
        rml << "<div id=\"" << id << "\" class=\"item\"><p>";
        rml << itemDesc;
        rml << "</p></div>";
    } else {
        rml << "<div id=\"" << id << "\" class=\"item\"><div><p>";
        rml << itemDesc;
        rml << "</p></div><div><p>";
        rml << shortcutDesc;
        rml << "</p></div></div>";
    }
}