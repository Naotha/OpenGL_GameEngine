#include "Application.h"
#include "Window/Window.h"
#include "EventHandler/EventHandler.h"

Application::Application(Window& window) : window(window) {
    renderer = Renderer::GetInstance();
    Init();
};

void Application::Init() {
    EventHandler::Init();
}

void Application::Run() {
    Setup();
    while (window.IsAlive())
    {
        OnLoop();
    }
    window.Shutdown();
}
