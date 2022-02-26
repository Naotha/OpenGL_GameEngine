#ifndef OPENGL_GAMEENGINE_WINDOW_H
#define OPENGL_GAMEENGINE_WINDOW_H


#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <string>
#include "Core/OpenGLContext.h"

class Window {
public:
    Window(std::string title, unsigned int windowWidth, unsigned int windowHeight);

    void OnUpdate();
    bool IsAlive();
    void Shutdown();

    void SetWidth(unsigned int newWidth) { _windowWidth = newWidth; }
    unsigned int GetWidth() const { return _windowWidth; }
    void SetHeight(unsigned int newHeight) { _windowHeight = newHeight; }
    unsigned int GetHeight() const { return _windowHeight; }

    GLFWwindow* GetGLFWWindow() { return _glfwWindow; }

private:
    void Init();
    void SetupCallbacks();

    std::string _title;

    unsigned int _windowWidth = 1280;
    unsigned int _windowHeight = 720;

    GLFWwindow* _glfwWindow;
    OpenGLContext _openglContext;
};


#endif //OPENGL_GAMEENGINE_WINDOW_H
