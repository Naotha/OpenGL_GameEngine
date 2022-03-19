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
};


#endif //OPENGL_GAMEENGINE_APPLICATION_H
