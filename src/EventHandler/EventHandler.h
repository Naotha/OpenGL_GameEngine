#ifndef OPENGL_GAMEENGINE_EVENTHANDLER_H
#define OPENGL_GAMEENGINE_EVENTHANDLER_H

#define GLFW_INCLUDE_NONE
#include <vector>
#include <GLFW/glfw3.h>
#include "Window/Window.h"
#include <iostream>
#include <colony/plf_colony.h>

class FrameBufferSizeEventCall
{
public:
    virtual void call(GLFWwindow* glfwWindow, int width, int height) = 0;
};

template<typename O>
class FrameBufferSizeEvent : public FrameBufferSizeEventCall {
public:
    FrameBufferSizeEvent(O* object, void (O::*function)(GLFWwindow* glfwWindow, int width, int height)) : object(object), function(function) {};

    void call(GLFWwindow* glfwWindow, int width, int height) override
    {
        (object->*function)(glfwWindow, width, height);
    }

    O* object;
    void (O::*function)(GLFWwindow* glfwWindow, int width, int height);
};

class CursorPosEventCall
{
public:
    virtual void call(GLFWwindow* glfwWindow, double xpos, double ypos) = 0;
};

template<typename O>
class CursorPosEvent : public CursorPosEventCall {
public:
    CursorPosEvent(O* object, void (O::*function)(GLFWwindow* glfwWindow, double xpos, double ypos)) : object(object), function(function) {};

    void call(GLFWwindow* glfwWindow, double xpos, double ypos) override
    {
        (object->*function)(glfwWindow, xpos, ypos);
    }

    O* object;
    void (O::*function)(GLFWwindow* glfwWindow, double xpos, double ypos);
};

class MouseButtonEventCall
{
public:
    virtual void call(GLFWwindow* glfwWindow, int button, int action, int mods) = 0;
};

template<typename O>
class MouseButtonEvent : public MouseButtonEventCall {
public:
    MouseButtonEvent(O* object, void (O::*function)(GLFWwindow* glfwWindow, int button, int action, int mods)) : object(object), function(function) {};

    void call(GLFWwindow* glfwWindow, int button, int action, int mods) override
    {
        (object->*function)(glfwWindow, button, action, mods);
    }

    O* object;
    void (O::*function)(GLFWwindow* glfwWindow, int button, int action, int mods);
};

class MouseScrollEventCall
{
public:
    virtual void call(GLFWwindow* glfwWindow, double xOffset, double yOffset) = 0;
};

template<typename O>
class MouseScrollEvent : public MouseScrollEventCall{
public:
    MouseScrollEvent(O* object, void (O::*function)(GLFWwindow* glfwWindow, double xOffset, double yOffset)) : object(object), function(function) {};

    void call(GLFWwindow* glfwWindow, double xOffset, double yOffset) override
    {
        (object->*function)(glfwWindow, xOffset, yOffset);
    }

    O* object;
    void (O::*function)(GLFWwindow* glfwWindow, double xOffset, double yOffset);
};

class KeyEventCall
{
public:
    virtual void call(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods) = 0;
};

template<typename O>
class KeyEvent : public KeyEventCall {
public:
    KeyEvent(O* object, void (O::*function)(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods)) : object(object), function(function) {};

    void call(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods) override
    {
        (object->*function)(glfwWindow, key, scancode, action, mods);
    }

    O* object;
    void (O::*function)(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods);
};

class EventHandler {
public:
    static void SetFrameBufferSizeCallback(FrameBufferSizeEventCall* event);

    template<class O>
    static void SetFrameBufferSizeCallback(O* object, void (O::*function)(GLFWwindow* glfwWindow, int width, int height))
    {
        _frameBufferSizeCallbacks.insert(new FrameBufferSizeEvent(object, function));
    }

    static void SetCursorPosCallback(CursorPosEventCall* event);

    template<class O>
    static void SetCursorPosCallback(O* object, void (O::*function)(GLFWwindow* glfwWindow, double xpos, double ypos))
    {
        _cursorPosCallbacks.insert(new CursorPosEvent(object, function));
    }

    static void SetMouseButtonCallback(MouseButtonEventCall* event);

    template<class O>
    static void SetMouseButtonCallback(O* object, void (O::*function)(GLFWwindow* glfwWindow, int button, int action, int mods))
    {
        _mouseButtonCallbacks.insert(new MouseButtonEvent(object, function));
    }

    static void SetMouseScrollCallback(MouseScrollEventCall* event);

    template<class O>
    static void SetMouseScrollCallback(O* object, void (O::*function)(GLFWwindow* glfwWindow, double xOffset, double yOffset))
    {
        _mouseScrollCallbacks.insert(new MouseScrollEvent(object, function));
    }
    
    static void SetKeyCallback(KeyEventCall* event);

    template<class O>
    static void SetKeyCallback(O* object, void (O::*function)(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods))
    {
        _keyCallbacks.insert(new KeyEvent(object, function));
    }

    static void Init();

private:
    EventHandler() = default;

    static void FrameBufferSizeCallback(GLFWwindow* glfwWindow, int width, int height);
    static void CursorPosCallback(GLFWwindow* glfwWindow, double xpos, double ypos);
    static void MouseButtonCallback(GLFWwindow* glfwWindow, int button, int action, int mods);
    static void MouseScrollCallback(GLFWwindow* glfwWindow, double xOffset, double yOffset);
    static void KeyCallback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods);

    inline static plf::colony<FrameBufferSizeEventCall*> _frameBufferSizeCallbacks;
    inline static plf::colony<CursorPosEventCall*> _cursorPosCallbacks;
    inline static plf::colony<MouseButtonEventCall*> _mouseButtonCallbacks;
    inline static plf::colony<MouseScrollEventCall*> _mouseScrollCallbacks;
    inline static plf::colony<KeyEventCall*> _keyCallbacks;
};


#endif //OPENGL_GAMEENGINE_EVENTHANDLER_H
