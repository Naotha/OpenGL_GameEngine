#ifndef OPENGL_GAMEENGINE_EVENTHANDLER_H
#define OPENGL_GAMEENGINE_EVENTHANDLER_H

#define GLFW_INCLUDE_NONE
#include <vector>
#include <GLFW/glfw3.h>
#include "Window/Window.h"

typedef GLFWframebuffersizefun  FrameBufferSizeEvent;
typedef GLFWcursorposfun CursorPosEvent;
typedef GLFWmousebuttonfun MouseButtonEvent;
typedef GLFWscrollfun MouseScrollEvent;
typedef GLFWkeyfun KeyEvent;

class EventHandler {
public:
    static void SetFrameBufferSizeCallback(FrameBufferSizeEvent event);
    static void SetCursorPosCallback(CursorPosEvent event);
    static void SetMouseButtonCallback(MouseButtonEvent event);
    static void SetMouseScrollCallback(MouseScrollEvent event);
    static void SetKeyCallback(KeyEvent event);

    static void Init();

private:
    EventHandler() = default;

    static void FrameBufferSizeCallback(GLFWwindow* glfwWindow, int width, int height);
    static void CursorPosCallback(GLFWwindow* glfwWindow, double xpos, double ypos);
    static void MouseButtonCallback(GLFWwindow* glfwWindow, int button, int action, int mods);
    static void MouseScrollCallback(GLFWwindow* glfwWindow, double xOffset, double yOffset);
    static void KeyCallback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods);

    inline static std::vector<FrameBufferSizeEvent> _frameBufferSizeCallbacks;
    inline static std::vector<CursorPosEvent> _cursorPosCallbacks;
    inline static std::vector<MouseButtonEvent> _mouseButtonCallbacks;
    inline static std::vector<MouseScrollEvent> _mouseScrollCallbacks;
    inline static std::vector<KeyEvent> _keyCallbacks;
};


#endif //OPENGL_GAMEENGINE_EVENTHANDLER_H
