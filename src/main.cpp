#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cmath>

#include "Engine/shader.hpp"
#include "Engine/mesh.hpp"

const unsigned short windowWidth = 800;
const unsigned short windowHeight = 600;

void frambuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

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

float cubeVertices[] = {
    /* Front Face */
    // Positions          // Colors            // Texture Coords
    -0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  // Front Top Left
    -0.5f, -0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  0.0f, 0.0f,  // Front Bottom Left
     0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  1.0f, 0.0f,  // Front Bottom Right
     0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  1.0f,  1.0f, 1.0f,  // Front Top Right
    /* Back Face */
    // Positions          // Colors            // Texture Coords
     0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  1.0f,  1.0f, 1.0f,  // Back Top Left
     0.5f, -0.5f, -0.5f,   0.0f,  0.0f,  1.0f,  1.0f, 0.0f,  // Back Bottom Left
    -0.5f, -0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  0.0f, 0.0f,  // Back Bottom Right
    -0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  // Back Top Right
    /* Top Face */
    // Positions          // Colors            // Texture Coords
    -0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  // Top Left
    -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  0.0f, 0.0f,  // Bottom Left
     0.5f,  0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  1.0f, 0.0f,  // Bottom Right
     0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  1.0f,  1.0f, 1.0f,  // Top Right
    /* Bottom Face */
    // Positions          // Colors            // Texture Coords
     0.5f, -0.5f, -0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  // Top Left
     0.5f, -0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  0.0f, 0.0f,  // Bottom Left
    -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  1.0f, 0.0f,  // Bottom Right
    -0.5f, -0.5f, -0.5f,   0.0f,  1.0f,  1.0f,  1.0f, 1.0f,  // Top Right
    /* Left Face */
    // Positions          // Colors            // Texture Coords
    -0.5f,  0.5f, -0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  // Top Left
    -0.5f, -0.5f, -0.5f,   0.0f,  1.0f,  0.0f,  0.0f, 0.0f,  // Bottom Left
    -0.5f, -0.5f,  0.5f,   0.0f,  0.0f,  1.0f,  1.0f, 0.0f,  // Bottom Right
    -0.5f,  0.5f,  0.5f,   0.0f,  1.0f,  1.0f,  1.0f, 1.0f,  // Top Right
    /* Right Face */
    // Positions          // Colors            // Texture Coords
     0.5f,  0.5f,  0.5f,   1.0f,  0.0f,  0.0f,  0.0f, 1.0f,  // Top Left
     0.5f, -0.5f,  0.5f,   0.0f,  1.0f,  0.0f,  0.0f, 0.0f,  // Bottom Left
     0.5f, -0.5f, -0.5f,   0.0f,  0.0f,  1.0f,  1.0f, 0.0f,  // Bottom Right
     0.5f,  0.5f, -0.5f,   0.0f,  1.0f,  1.0f,  1.0f, 1.0f,  // Top Right
};

unsigned int triangleIndices[] = {
    0, 1, 2,
};

unsigned int squareIndices[] = {
    0, 1, 2,
    2, 3, 0
};

unsigned int cubeIndices[] = {
    // Front Face
     0,  1,  2,
     2,  3,  0,
    // Back Face
     4,  5,  6,
     6,  7,  4,
    // Top Face
     8,  9, 10,
    10, 11,  8,
    // Bottom Face
    12, 13, 14,
    14, 15, 12,
    // Left Face
    16, 17, 18,
    18, 19, 16,
    // Right Face
    20, 21, 22,
    22, 23, 20,
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

    /* GL Enable */
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    /* Set Viewport */
    glViewport(0, 0, windowWidth, windowHeight);
    glfwSetFramebufferSizeCallback(window, frambuffer_size_callback); // Change viewport when window is resized
    
    /* Initialize Shaders */
    Shader basicShader("./shaders/shader.vert", "./shaders/shader.frag");

    /* Create Objects */
    Mesh square(squareVertices, sizeof(squareVertices), squareIndices, sizeof(squareIndices), &basicShader);
    square.addTextureUnit("./textures/bricks.jpg");
    square.addTextureUnit("./textures/blastoise.png", GL_RGBA);

    Mesh cube(cubeVertices, sizeof(cubeVertices), cubeIndices, sizeof(cubeIndices), &basicShader);
    cube.addTextureUnit("./textures/bricks.jpg");
    cube.addTextureUnit("./textures/blastoise.png", GL_RGBA);

    /* Set samplers to the corresponding texture unit */
    basicShader.use();
    glUniform1i(glGetUniformLocation(basicShader.getID(), "tex1"), 0);
    glUniform1i(glGetUniformLocation(basicShader.getID(), "tex2"), 1);

    /* Transformations */
    glm::mat4 model = glm::mat4(1.0f);
    
    glm::mat4 view = glm::mat4(1.0f);
    view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
    
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);

    basicShader.setViewMatrix(view, "view");
    basicShader.setProjectionMatrix(projection, "projection");

    /* Loop until the user closes the window - Render Loop */
    while (!glfwWindowShouldClose(window))
    {
        /* User inputs */
        processInput(window);
        
        /* Rendering */
        glClearColor(0.1f,0.4f,0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Transformations */
        model = glm::rotate(model, glm::radians(1.0f), glm::vec3(1.0f, 0.3f, 0.5f));
        cube.transform(model);
        /* Draw Objects */
        cube.draw();

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