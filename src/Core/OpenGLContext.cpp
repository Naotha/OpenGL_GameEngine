#include "Core/OpenGLContext.h"
#include <iostream>

void OpenGLContext::Init(GLFWwindow *glfwWindow)
{
    _glfwWindow = glfwWindow;
    glfwMakeContextCurrent(_glfwWindow);

    /* Initialize GLAD */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD!" << std::endl;
    }

    /* GL Enable */
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

void OpenGLContext::SwapBuffers()
{
    glfwSwapBuffers(_glfwWindow);
}