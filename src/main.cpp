#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <ImGui/imgui.h>
#include <ImGui/backends/imgui_impl_glfw.h>
#include <ImGui/backends/imgui_impl_opengl3.h>

#include <iostream>
#include <vector>

#include "Engine/shader.hpp"
#include "Engine/texture.hpp"
#include "Engine/mesh.hpp"
#include "Engine/vertex.h"
#include "Engine/camera.hpp"


void frambuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void calculateDeltaTime();

/* Window, cursor */
unsigned short windowWidth = 800;
unsigned short windowHeight = 600;
float lastX = windowWidth / 2;
float lastY = windowHeight / 2;
bool firstMouse = true;
bool cameraMode = false;

/* Time */
float deltaTime = 0.0f;
float lastFrame = 0.0f;

/* Camera */
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

/* Vertices */
std::vector<Vertex> triangleVertices = {
    // Postitions                      // Colors                        // Texture Coords
    {glm::vec3(-0.5f, -0.5f,  0.0f),   glm::vec3(1.0f,  0.0f,  0.0f),   glm::vec2(0.0f, 0.0f)},  // Bottom Left
    {glm::vec3( 0.5f, -0.5f,  0.0f),   glm::vec3(0.0f,  1.0f,  0.0f),   glm::vec2(1.0f, 0.0f)},  // Bottom Right
    {glm::vec3( 0.0f,  0.5f,  0.0f),   glm::vec3(0.0f,  0.0f,  1.0f),   glm::vec2(0.5f, 1.0f)}   // Top 
};

std::vector<Vertex> squareVertices = {
    // Positions                       // Colors                       // Texture Coords
    {glm::vec3(-0.5f,  0.5f,  0.0f),   glm::vec3(1.0f,  0.0f,  0.0f),   glm::vec2(0.0f, 1.0f)},  // Top Left
    {glm::vec3(-0.5f, -0.5f,  0.0f),   glm::vec3(0.0f,  1.0f,  0.0f),   glm::vec2(0.0f, 0.0f)},  // Bottom Left
    {glm::vec3( 0.5f, -0.5f,  0.0f),   glm::vec3(0.0f,  0.0f,  1.0f),   glm::vec2(1.0f, 0.0f)},  // Bottom Right
    {glm::vec3( 0.5f,  0.5f,  0.0f),   glm::vec3(0.0f,  1.0f,  1.0f),   glm::vec2(1.0f, 1.0f)}   // Top Right
};

std::vector<Vertex> cubeVertices = {
    /* Front Face */
    // Positions                       // Colors                       // Texture Coords
    {glm::vec3(-0.5f,  0.5f,  0.5f),   glm::vec3(1.0f,  0.0f,  0.0f),   glm::vec2(0.0f, 1.0f)},  // Front Top Left
    {glm::vec3(-0.5f, -0.5f,  0.5f),   glm::vec3(0.0f,  1.0f,  0.0f),   glm::vec2(0.0f, 0.0f)},  // Front Bottom Left
    {glm::vec3( 0.5f, -0.5f,  0.5f),   glm::vec3(0.0f,  0.0f,  1.0f),   glm::vec2(1.0f, 0.0f)},  // Front Bottom Right
    {glm::vec3( 0.5f,  0.5f,  0.5f),   glm::vec3(0.0f,  1.0f,  1.0f),   glm::vec2(1.0f, 1.0f)},  // Front Top Right
    /* Back Face */
    // Positions                       // Colors                        // Texture Coords
    {glm::vec3( 0.5f,  0.5f, -0.5f),   glm::vec3(0.0f,  1.0f,  1.0f),   glm::vec2(1.0f, 1.0f)},  // Back Top Left
    {glm::vec3( 0.5f, -0.5f, -0.5f),   glm::vec3(0.0f,  0.0f,  1.0f),   glm::vec2(1.0f, 0.0f)},  // Back Bottom Left
    {glm::vec3(-0.5f, -0.5f, -0.5f),   glm::vec3(0.0f,  1.0f,  0.0f),   glm::vec2(0.0f, 0.0f)},  // Back Bottom Right
    {glm::vec3(-0.5f,  0.5f, -0.5f),   glm::vec3(1.0f,  0.0f,  0.0f),   glm::vec2(0.0f, 1.0f)},  // Back Top Right
    /* Top Face */
    // Positions                       // Colors                        // Texture Coords
    {glm::vec3(-0.5f,  0.5f, -0.5f),   glm::vec3(1.0f,  0.0f,  0.0f),   glm::vec2(0.0f, 1.0f)},  // Top Left
    {glm::vec3(-0.5f,  0.5f,  0.5f),   glm::vec3(0.0f,  1.0f,  0.0f),   glm::vec2(0.0f, 0.0f)},  // Bottom Left
    {glm::vec3( 0.5f,  0.5f,  0.5f),   glm::vec3(0.0f,  0.0f,  1.0f),   glm::vec2(1.0f, 0.0f)},  // Bottom Right
    {glm::vec3( 0.5f,  0.5f, -0.5f),   glm::vec3(0.0f,  1.0f,  1.0f),   glm::vec2(1.0f, 1.0f)},  // Top Right
    /* Bottom Face */
    // Positions                       // Colors                        // Texture Coords
    {glm::vec3( 0.5f, -0.5f, -0.5f),   glm::vec3(1.0f,  0.0f,  0.0f),   glm::vec2(0.0f, 1.0f)},  // Top Left
    {glm::vec3( 0.5f, -0.5f,  0.5f),   glm::vec3(0.0f,  1.0f,  0.0f),   glm::vec2(0.0f, 0.0f)},  // Bottom Left
    {glm::vec3(-0.5f, -0.5f,  0.5f),   glm::vec3(0.0f,  0.0f,  1.0f),   glm::vec2(1.0f, 0.0f)},  // Bottom Right
    {glm::vec3(-0.5f, -0.5f, -0.5f),   glm::vec3(0.0f,  1.0f,  1.0f),   glm::vec2(1.0f, 1.0f)},  // Top Right
    /* Left Face */
    // Positions                       // Colors                        // Texture Coords
    {glm::vec3(-0.5f,  0.5f, -0.5f),   glm::vec3(1.0f,  0.0f,  0.0f),   glm::vec2(0.0f, 1.0f)},  // Top Left
    {glm::vec3(-0.5f, -0.5f, -0.5f),   glm::vec3(0.0f,  1.0f,  0.0f),   glm::vec2(0.0f, 0.0f)},  // Bottom Left
    {glm::vec3(-0.5f, -0.5f,  0.5f),   glm::vec3(0.0f,  0.0f,  1.0f),   glm::vec2(1.0f, 0.0f)},  // Bottom Right
    {glm::vec3(-0.5f,  0.5f,  0.5f),   glm::vec3(0.0f,  1.0f,  1.0f),   glm::vec2(1.0f, 1.0f)},  // Top Right
    /* Right Face */
    // Positions                       // Colors                        // Texture Coords
    {glm::vec3( 0.5f,  0.5f,  0.5f),   glm::vec3(1.0f,  0.0f,  0.0f),   glm::vec2(0.0f, 1.0f)},  // Top Left
    {glm::vec3( 0.5f, -0.5f,  0.5f),   glm::vec3(0.0f,  1.0f,  0.0f),   glm::vec2(0.0f, 0.0f)},  // Bottom Left
    {glm::vec3( 0.5f, -0.5f, -0.5f),   glm::vec3(0.0f,  0.0f,  1.0f),   glm::vec2(1.0f, 0.0f)},  // Bottom Right
    {glm::vec3( 0.5f,  0.5f, -0.5f),   glm::vec3(0.0f,  1.0f,  1.0f),   glm::vec2(1.0f, 1.0f)},  // Top Right
};

std::vector<unsigned int> triangleIndices = {
    0, 1, 2,
};

std::vector<unsigned int> squareIndices = {
    0, 1, 2,
    2, 3, 0
};

std::vector<unsigned int> cubeIndices = {
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
    /// SETUP GLFW
    /* Initialize the library */
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL Engine", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /// SETUP IMGUI
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    

    /// SETUP OPENGL
    /* Initialize GLAD */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    /* GL Enable */
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    /* Set Viewport */
    glViewport(0, 0, windowWidth, windowHeight);
    glfwSetFramebufferSizeCallback(window, frambuffer_size_callback); // Change viewport when window is resized
    
    /* Set Mouse callbacks */
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    /* Initialize Shaders */
    Shader basicShader("./shaders/shader.vert", "./shaders/shader.frag");
    Shader basicShader2("./shaders/shader.vert", "./shaders/shader.frag");
    
    /* Create Objects */
    Texture brickTexture("./textures/bricks.jpg", "u_tex0");
    Texture blastoiseTexture("./textures/blastoise.png", "u_tex1", GL_RGBA);
    std::vector<Texture> cubeTextures = {brickTexture, blastoiseTexture};
    std::vector<Texture> cube2Textures = {brickTexture};

    Mesh cube(cubeVertices, cubeIndices, cubeTextures, &basicShader);

    Mesh cube2(cubeVertices, cubeIndices, &basicShader2);
    cube2.setTextures(cube2Textures);

    /* Transformation Matrices */
    glm::mat4 model = glm::mat4(1.0f);

    /* Loop until the user closes the window - Render Loop */
    while (!glfwWindowShouldClose(window))
    {
        /* User inputs */
        processInput(window);
        
        /* Camera Calculations */
        glm::mat4 projection = glm::perspective(glm::radians(camera.getFOV()),
                                                (float)windowWidth / (float)windowHeight,
                                                0.1f, 100.0f);
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 mvp = projection * view * model;

        /* Rendering */
        glClearColor(0.1f,0.4f,0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Draw Objects */
        for (int i = 0; i < 10; i++)
        {   
            basicShader.use();
            glm::mat4 model_i = glm::translate(model, glm::vec3(0.0f, 0.0f, -1.0f * i));
            glm::mat4 mvp = projection * view * model_i;

            basicShader.setUniformMat4("u_mvp", mvp);
            
            cube.draw();
        }
        
        for (int i = 0; i < 10; i++)
        {
            basicShader2.use();
            glm::mat4 model_i = glm::translate(model, glm::vec3(0.0f, 2.0f, -1.0f * i));
            glm::mat4 mvp = projection * view * model_i;

            basicShader2.setUniformMat4("u_mvp", mvp);
            
            cube2.draw();
        }

        /* ImGUI */
        // feed inputs to dear imgui, start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Begin("Demo window");
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_RELEASE)
    {
        cameraMode = false;
        firstMouse = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    
    if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
    {   
        if (cameraMode == false)
        {
            cameraMode = true;
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.processKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.processKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.processKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.processKeyboard(RIGHT, deltaTime);
    }
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{   
    if (cameraMode)
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

        camera.processMouseMovement(xOffset, yOffset);
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.processMouseScroll(yoffset);
}

void calculateDeltaTime()
{
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}