#ifndef OPENGL_GAMEENGINE_OPENGLCONTEXT_H
#define OPENGL_GAMEENGINE_OPENGLCONTEXT_H


#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

class OpenGLContext {
public:
    OpenGLContext() {}

    void Init(GLFWwindow* glfwWindow);
    void SwapBuffers();

    int GetMajorVersion() { return  _majorVersion; }
    int GetMinorVersion() { return  _minorVersion; }

private:
    int _majorVersion = 4;
    int _minorVersion = 6;

    GLFWwindow* _glfwWindow = nullptr;
};


#endif //OPENGL_GAMEENGINE_OPENGLCONTEXT_H
