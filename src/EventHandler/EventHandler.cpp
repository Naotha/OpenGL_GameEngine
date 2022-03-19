#include "EventHandler.h"
#include <iostream>

void EventHandler::SetFrameBufferSizeCallback(FrameBufferSizeEventCall* event) {
    _frameBufferSizeCallbacks.push_back(event);
}

void EventHandler::SetCursorPosCallback(CursorPosEventCall* event) {
    _cursorPosCallbacks.push_back(event);
}

void EventHandler::SetMouseButtonCallback(MouseButtonEventCall* event) {
    _mouseButtonCallbacks.push_back(event);
}

void EventHandler::SetMouseScrollCallback(MouseScrollEventCall* event) {
    _mouseScrollCallbacks.push_back(event);
}

void EventHandler::SetKeyCallback(KeyEventCall* event) {
    _keyCallbacks.push_back(event);
}

void EventHandler::FrameBufferSizeCallback(GLFWwindow* glfwWindow, int width, int height) {
    for (FrameBufferSizeEventCall* event : _frameBufferSizeCallbacks)
    {
        event->call(glfwWindow, width, height);
    }
}

void EventHandler::CursorPosCallback(GLFWwindow* glfwWindow, double xpos, double ypos) {
    for (CursorPosEventCall* event : _cursorPosCallbacks)
    {
        event->call(glfwWindow, xpos, ypos);
    }
}

void EventHandler::MouseButtonCallback(GLFWwindow* glfwWindow, int button, int action, int mods) {
    for (MouseButtonEventCall* event : _mouseButtonCallbacks)
    {
        event->call(glfwWindow, button, action, mods);
    }
}

void EventHandler::MouseScrollCallback(GLFWwindow* glfwWindow, double xoffset, double yoffset) {
    for (MouseScrollEventCall* event : _mouseScrollCallbacks)
    {
        event->call(glfwWindow, xoffset, yoffset);
    }
}

void EventHandler::KeyCallback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods) {
    for (KeyEventCall* event : _keyCallbacks)
    {
        event->call(glfwWindow, key, scancode, action, mods);
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
