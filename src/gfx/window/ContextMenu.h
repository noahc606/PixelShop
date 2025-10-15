#pragma once
#include "Window.h"
#include <nch/math-utils/vec2.h>
#include <nch/rmlui-utils/web-events.h>

class ContextMenu : public Window {
public:
    enum ItemSet {
        FILE, EDIT
    };

    ContextMenu(const nch::Vec2i& spawnPos = {0, 30}, int itemSet = 0);
    ~ContextMenu();
    void reload();
    void tick();
    void draw();
    
    nch::Vec2i getSpawnPos();
private:
    void addItemToRML(std::stringstream& rml, std::string id, std::string itemDesc, std::string shortcutDesc = "");

    int itemSet = 0;
    nch::Vec2i spawnPos = {0, 0};
    static std::vector<std::map<std::string, std::vector<std::string>>> itemSets;
};