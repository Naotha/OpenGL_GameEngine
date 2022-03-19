#include "Application.h"
#include "Window/Window.h"
#include "EventHandler/EventHandler.h"

Application::Application(Window& window) : window(window) {
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
