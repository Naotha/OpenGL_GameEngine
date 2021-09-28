#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cmath>

#include "Engine/shader.hpp"
#include "Engine/mesh.hpp"


void frambuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void calculateDeltaTime();

/* Window, cursor */
const unsigned short windowWidth = 800;
const unsigned short windowHeight = 600;
float lastX = windowWidth / 2;
float lastY = windowHeight / 2;
bool firstMouse = true;

/* Time */
float deltaTime = 0.0f;
float lastFrame = 0.0f;

/* Camera */
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float pitch;
float yaw = -90.0f;
float fov = 45.0f;

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
    
    /* Set Cursor mode */
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
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

    /* Transformation Matrices */
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),
                                 glm::vec3(0.0f, 0.0f, 0.0f),
                                 glm::vec3(0.0f, 1.0f, 0.0f));

    /* Loop until the user closes the window - Render Loop */
    while (!glfwWindowShouldClose(window))
    {
        /* User inputs */
        processInput(window);
        
        /* Camera Calculations */
        glm::mat4 projection = glm::perspective(glm::radians(fov), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
        basicShader.setProjectionMatrix(projection, "projection");
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
        basicShader.setViewMatrix(view, "view");
        /* Rendering */
        glClearColor(0.1f,0.4f,0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Transformations */
        // model = glm::rotate(model, glm::radians(1.0f), glm::vec3(1.0f, 0.3f, 0.5f));
        // cube.transform(model);
        /* Draw Objects */
        cube.draw();

        /* Poll for and process events */
        glfwPollEvents();
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        /* Calculate time between frames */
        calculateDeltaTime();
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
    const float cameraSpeed = 2.5f * deltaTime;
    glm::vec3 cameraRight = glm::normalize(glm::cross(cameraFront, cameraUp));
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPos -= cameraRight * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPos += cameraRight * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{   
    // Circumvent sudden jump on focus gain
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    // Calculate mouse offset
    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    const float sensitivity = 0.1f;
    xOffset *= sensitivity;
    yOffset *= sensitivity;

    // Set Yaw and Pitch
    yaw += xOffset;
    pitch += yOffset;
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
    
    // Calculate camera direction
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(direction);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    fov -= (float)yoffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

void calculateDeltaTime()
{
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}