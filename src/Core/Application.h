#ifndef OPENGL_GAMEENGINE_APPLICATION_H
#define OPENGL_GAMEENGINE_APPLICATION_H

#include "Window/Window.h"
#include "Renderer/Renderer.h"

class Application {
public:
    Application(Window& window);

    void Run();
    virtual void OnLoop() = 0;
    virtual void Setup() = 0;

protected:
    void Init();

    Window& window;
    Renderer* renderer;
};

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

#endif //OPENGL_GAMEENGINE_APPLICATION_H
