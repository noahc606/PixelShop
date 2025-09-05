#include "Window.h"
using namespace nch;
int Window::latestWindowID = -1;
SDL_Renderer* Window::rend = nullptr;

void Window::initRenderer(SDL_Renderer* rend) {
    Window::rend = rend;
}

void Window::super_init(const std::string& webdocAssetPath, nch::Vec2i dims)
{
    latestWindowID++;

    std::stringstream ctxID; ctxID << "generic_window_" << latestWindowID;
    webview.initContext(ctxID.str());
    webview.rmlLoadDocumentAsset(webdocAssetPath);
    webview.resize(dims);
}

void Window::tick(){}
void Window::draw(){}
void Window::reload()
{
    webview.reload();
}