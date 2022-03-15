#include "EventHandler.h"
#include <iostream>

void EventHandler::SetFrameBufferSizeCallback(FrameBufferSizeEvent event) {
    _frameBufferSizeCallbacks.push_back(event);
}

void EventHandler::SetCursorPosCallback(CursorPosEvent event) {
    _cursorPosCallbacks.push_back(event);
}

void EventHandler::SetMouseButtonCallback(MouseButtonEvent event) {
    _mouseButtonCallbacks.push_back(event);
}

void EventHandler::SetMouseScrollCallback(MouseScrollEvent event) {
    _mouseScrollCallbacks.push_back(event);
}

void EventHandler::SetKeyCallback(KeyEvent event) {
    _keyCallbacks.push_back(event);
}

void EventHandler::FrameBufferSizeCallback(GLFWwindow* glfwWindow, int width, int height) {
    for (FrameBufferSizeEvent event : _frameBufferSizeCallbacks)
    {
        event(glfwWindow, height, width);
    }
}

void EventHandler::CursorPosCallback(GLFWwindow* glfwWindow, double xpos, double ypos) {
    for (CursorPosEvent event : _cursorPosCallbacks)
    {
        event(glfwWindow, xpos, ypos);
    }
}

void EventHandler::MouseButtonCallback(GLFWwindow* glfwWindow, int button, int action, int mods) {
    for (MouseButtonEvent event : _mouseButtonCallbacks)
    {
        event(glfwWindow, button, action, mods);
    }
}

void EventHandler::MouseScrollCallback(GLFWwindow* glfwWindow, double xoffset, double yoffset) {
    for (MouseScrollEvent event : _mouseScrollCallbacks)
    {
        event(glfwWindow, xoffset, yoffset);
    }
}

void EventHandler::KeyCallback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods) {
    for (KeyEvent event : _keyCallbacks)
    {
        event(glfwWindow, key, scancode, action, mods);
    }
}

void EventHandler::Init() {
    Window& window = Window::GetInstance();
    glfwSetFramebufferSizeCallback(window.GetGLFWWindow(), FrameBufferSizeCallback);
    glfwSetCursorPosCallback(window.GetGLFWWindow(), CursorPosCallback);
    glfwSetMouseButtonCallback(window.GetGLFWWindow(), MouseButtonCallback);
    glfwSetScrollCallback(window.GetGLFWWindow(), MouseScrollCallback);
    glfwSetKeyCallback(window.GetGLFWWindow(), KeyCallback);
}
