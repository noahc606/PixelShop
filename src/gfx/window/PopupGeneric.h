#pragma once
#include <string>
#include "Window.h"

class PopupGeneric : public Window {
public:
    PopupGeneric(const std::string& webdocAssetPath);
    ~PopupGeneric();

    void reload();
    void tick();
    void draw();

    std::string getWebdocAssetPath();
private:
    std::string webdocAssetPath;
};