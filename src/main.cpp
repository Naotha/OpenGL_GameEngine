#include "glad/glad.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <cmath>

#include "Engine/shader.hpp"
#include "Engine/mesh.hpp"

const unsigned short windowWidth = 800;
const unsigned short windowHeight = 600;

/* Vertices */
float triangleVertices[] = {
    // Postitions         // Colors             // Texture Coords
    -0.5f, -0.5f,  0.0f,   1.0f,  0.0f,  0.0f,   0.0f, 0.0f,  // Bottom Left
     0.5f, -0.5f,  0.0f,   0.0f,  1.0f,  0.0f,   1.0f, 0.0f,  // Bottom Right
     0.0f,  0.5f,  0.0f,   0.0f,  0.0f,  1.0f,   0.5f, 1.0f   // Top 
};

float squareVertices[] = {
    // Positions          // Colors            // Texture Coords
    -0.5f,  0.5f,  0.0f,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  // Top Left
    -0.5f, -0.5f,  0.0f,   0.0f,  1.0f,  0.0f,  0.0f, 0.0f,  // Bottom Left
     0.5f, -0.5f,  0.0f,   0.0f,  0.0f,  1.0f,  1.0f, 0.0f,  // Bottom Right
     0.5f,  0.5f,  0.0f,   0.0f,  1.0f,  1.0f,  1.0f, 1.0f   // Top Right
};

unsigned int triangleIndices[] = {
    0, 1, 2,
};

unsigned int squareIndices[] = {
    0, 1, 2,
    2, 3, 0
};

int main(void)
{
    /* Initialize the library */
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Hello World", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Initialize GLAD */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    /* Enable polygon culling */
    glEnable(GL_CULL_FACE);

    /* Set Viewport */
    glViewport(0, 0, windowWidth, windowHeight);
    glfwSetFramebufferSizeCallback(window, frambuffer_size_callback); // Change viewport when window is resized
    
    /* Initialize Shaders */
    Shader basicShader("./shaders/shader.vert", "./shaders/shader.frag");

    Mesh square(squareVertices, sizeof(squareVertices), squareIndices, sizeof(squareIndices), &basicShader);
    square.addTextureUnit("./textures/bricks.jpg");
    square.addTextureUnit("./textures/blastoise.png", GL_RGBA);

    // Set samplers to the corresponding texture unit
    basicShader.use();
    glUniform1i(glGetUniformLocation(basicShader.getID(), "tex1"), 0);
    glUniform1i(glGetUniformLocation(basicShader.getID(), "tex2"), 1);

    /* Loop until the user closes the window - Render Loop */
    while (!glfwWindowShouldClose(window))
    {
        /* User inputs */
        processInput(window);
        
        /* Rendering */
        glClearColor(0.1f,0.4f,0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        /* Draw Objects */
        square.draw();

        /* Poll for and process events */
        glfwPollEvents();
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}

void frambuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}