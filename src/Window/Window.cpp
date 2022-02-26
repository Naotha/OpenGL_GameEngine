#include "Window/Window.h"
#include <iostream>

Window::Window(std::string title, unsigned int windowWidth, unsigned int windowHeight)
{
    _title = title;
    _windowWidth = windowWidth;
    _windowHeight = windowHeight;
    _openglContext = OpenGLContext();

    Init();
}

void Window::Init()
{
    if (!glfwInit())
        std::cout<<"Failed to initialize GLFW!"<<std::endl;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, _openglContext.GetMajorVersion());
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, _openglContext.GetMinorVersion());
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    _glfwWindow = glfwCreateWindow((int)_windowWidth, (int)_windowHeight, _title.c_str(), nullptr, nullptr);
    if (!_glfwWindow)
    {
        std::cout << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
    }

    _openglContext.Init(_glfwWindow);

    SetupCallbacks();
}

void Window::SetupCallbacks()
{

}

void Window::OnUpdate()
{
    glfwPollEvents();
    _openglContext.SwapBuffers();
}

void Window::Shutdown()
{
    glfwDestroyWindow(_glfwWindow);
    glfwTerminate();
}

bool Window::IsAlive()
{
    return !glfwWindowShouldClose(_glfwWindow);
}
