#include "PopupGeneric.h"

PopupGeneric::PopupGeneric(const std::string& webdocAssetPath)
{
    PopupGeneric::webdocAssetPath = webdocAssetPath;
    super_init(Window::POPUP_GENERIC, {480, 480});

    reload();
}

PopupGeneric::~PopupGeneric()
{}

void PopupGeneric::reload()
{
    super_reload();
}
void PopupGeneric::tick()
{
    super_tick();
}
void PopupGeneric::draw()
{
    super_draw();
}

std::string PopupGeneric::getWebdocAssetPath() {
    return webdocAssetPath;
}