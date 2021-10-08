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
#include "Engine/material.h"


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
    // Positions                       // Normals                       // Texture Coords
    {glm::vec3(-0.5f,  0.5f,  0.5f),   glm::vec3( 0.0f,  0.0f,  1.0f),   glm::vec2(0.0f, 1.0f)},  // Front Top Left
    {glm::vec3(-0.5f, -0.5f,  0.5f),   glm::vec3( 0.0f,  0.0f,  1.0f),   glm::vec2(0.0f, 0.0f)},  // Front Bottom Left
    {glm::vec3( 0.5f, -0.5f,  0.5f),   glm::vec3( 0.0f,  0.0f,  1.0f),   glm::vec2(1.0f, 0.0f)},  // Front Bottom Right
    {glm::vec3( 0.5f,  0.5f,  0.5f),   glm::vec3( 0.0f,  0.0f,  1.0f),   glm::vec2(1.0f, 1.0f)},  // Front Top Right
    /* Back Face */
    // Positions                       // Normals                        // Texture Coords
    {glm::vec3( 0.5f,  0.5f, -0.5f),   glm::vec3( 0.0f,  0.0f, -1.0f),   glm::vec2(1.0f, 1.0f)},  // Back Top Left
    {glm::vec3( 0.5f, -0.5f, -0.5f),   glm::vec3( 0.0f,  0.0f, -1.0f),   glm::vec2(1.0f, 0.0f)},  // Back Bottom Left
    {glm::vec3(-0.5f, -0.5f, -0.5f),   glm::vec3( 0.0f,  0.0f, -1.0f),   glm::vec2(0.0f, 0.0f)},  // Back Bottom Right
    {glm::vec3(-0.5f,  0.5f, -0.5f),   glm::vec3( 0.0f,  0.0f, -1.0f),   glm::vec2(0.0f, 1.0f)},  // Back Top Right
    /* Top Face */
    // Positions                       // Normals                        // Texture Coords
    {glm::vec3(-0.5f,  0.5f, -0.5f),   glm::vec3( 0.0f,  1.0f,  0.0f),   glm::vec2(0.0f, 1.0f)},  // Top Left
    {glm::vec3(-0.5f,  0.5f,  0.5f),   glm::vec3( 0.0f,  1.0f,  0.0f),   glm::vec2(0.0f, 0.0f)},  // Bottom Left
    {glm::vec3( 0.5f,  0.5f,  0.5f),   glm::vec3( 0.0f,  1.0f,  0.0f),   glm::vec2(1.0f, 0.0f)},  // Bottom Right
    {glm::vec3( 0.5f,  0.5f, -0.5f),   glm::vec3( 0.0f,  1.0f,  0.0f),   glm::vec2(1.0f, 1.0f)},  // Top Right
    /* Bottom Face */
    // Positions                       // Normals                        // Texture Coords
    {glm::vec3( 0.5f, -0.5f, -0.5f),   glm::vec3( 0.0f, -1.0f,  0.0f),   glm::vec2(0.0f, 1.0f)},  // Top Left
    {glm::vec3( 0.5f, -0.5f,  0.5f),   glm::vec3( 0.0f, -1.0f,  0.0f),   glm::vec2(0.0f, 0.0f)},  // Bottom Left
    {glm::vec3(-0.5f, -0.5f,  0.5f),   glm::vec3( 0.0f, -1.0f,  0.0f),   glm::vec2(1.0f, 0.0f)},  // Bottom Right
    {glm::vec3(-0.5f, -0.5f, -0.5f),   glm::vec3( 0.0f, -1.0f,  0.0f),   glm::vec2(1.0f, 1.0f)},  // Top Right
    /* Left Face */
    // Positions                       // Normals                        // Texture Coords
    {glm::vec3(-0.5f,  0.5f, -0.5f),   glm::vec3(-1.0f,  0.0f,  0.0f),   glm::vec2(0.0f, 1.0f)},  // Top Left
    {glm::vec3(-0.5f, -0.5f, -0.5f),   glm::vec3(-1.0f,  0.0f,  0.0f),   glm::vec2(0.0f, 0.0f)},  // Bottom Left
    {glm::vec3(-0.5f, -0.5f,  0.5f),   glm::vec3(-1.0f,  0.0f,  0.0f),   glm::vec2(1.0f, 0.0f)},  // Bottom Right
    {glm::vec3(-0.5f,  0.5f,  0.5f),   glm::vec3(-1.0f,  0.0f,  0.0f),   glm::vec2(1.0f, 1.0f)},  // Top Right
    /* Right Face */
    // Positions                       // Normals                        // Texture Coords
    {glm::vec3( 0.5f,  0.5f,  0.5f),   glm::vec3( 1.0f,  0.0f,  0.0f),   glm::vec2(0.0f, 1.0f)},  // Top Left
    {glm::vec3( 0.5f, -0.5f,  0.5f),   glm::vec3( 1.0f,  0.0f,  0.0f),   glm::vec2(0.0f, 0.0f)},  // Bottom Left
    {glm::vec3( 0.5f, -0.5f, -0.5f),   glm::vec3( 1.0f,  0.0f,  0.0f),   glm::vec2(1.0f, 0.0f)},  // Bottom Right
    {glm::vec3( 0.5f,  0.5f, -0.5f),   glm::vec3( 1.0f,  0.0f,  0.0f),   glm::vec2(1.0f, 1.0f)},  // Top Right
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

/* Materials */
std::vector<Material::Material> materials = {
    Material::EMERALD,
    Material::JADE,
    Material::OBSIDIAN,
    Material::PEARL,
    Material::RUBY,
    Material::TURQUOISE,
    Material::BRASS,
    Material::BRONZE,
    Material::CHROME,
    Material::COPPER,
    Material::GOLD,
    Material::SILVER,
    Material::BLACK_PLASTIC,
    Material::CYAN_PLASTIC,
    Material::GREEN_PLASTIC,
    Material::RED_PLASTIC,
    Material::WHITE_PLASTIC,
    Material::YELLOW_PLASTIC,
    Material::BLACK_RUBBER,
    Material::CYAN_RUBBER,
    Material::GREEN_RUBBER,
    Material::RED_RUBBER,
    Material::WHITE_RUBBER,
    Material::YELLOW_RUBBER
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
    // IMGUI_CHECKVERSION();
    // ImGui::CreateContext();
    // ImGuiIO &io = ImGui::GetIO();
    // // Setup Platform/Renderer bindings
    // ImGui_ImplGlfw_InitForOpenGL(window, true);
    // ImGui_ImplOpenGL3_Init();
    // // Setup Dear ImGui style
    // ImGui::StyleColorsDark();
    

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
    Shader basicShader("./shaders/basicShader.vert", "./shaders/basicShader.frag");
    Shader basicMaterialShader("./shaders/basicMaterialShader.vert", "./shaders/basicMaterialShader.frag");
    Shader lightSourceShader("./shaders/lightSourceShader.vert", "./shaders/lightSourceShader.frag");
    
    /* Create Objects */
    Texture brickTexture("./textures/bricks.jpg", "u_tex0");
    Texture blastoiseTexture("./textures/blastoise.png", "u_tex1", GL_RGBA);
    std::vector<Texture> cubeTextures = {brickTexture, blastoiseTexture};

    Mesh cube(cubeVertices, cubeIndices, cubeTextures, &basicMaterialShader);

    /* Light */
    Mesh lightCube(cubeVertices, cubeIndices, &lightSourceShader);
    glm::vec3 lightPos(2.0f, 2.0f, 2.0f);
    glm::vec3 lightCol(1.0f, 1.0f, 1.0f);
    basicMaterialShader.use();
    basicMaterialShader.setUniformFloat3("u_material.ambient", Material::EMERALD.ambient);
    basicMaterialShader.setUniformFloat3("u_material.diffuse", Material::EMERALD.diffuse);
    basicMaterialShader.setUniformFloat3("u_material.specular", Material::EMERALD.specular);
    basicMaterialShader.setUniformFloat("u_material.shininess", Material::EMERALD.shininess);
    glm::vec3 ambientColor = lightCol * glm::vec3(0.2f);
    glm::vec3 diffuseColor = lightCol * glm::vec3(0.8f);
    basicMaterialShader.setUniformFloat3("u_light.position", lightPos);
    basicMaterialShader.setUniformFloat3("u_light.ambient", ambientColor);
    basicMaterialShader.setUniformFloat3("u_light.diffuse", diffuseColor);
    basicMaterialShader.setUniformFloat3("u_light.specular", lightCol);
    lightSourceShader.use();
    lightSourceShader.setUniformFloat3("u_lightCol", lightCol);

    /* Transformation Matrices */
    glm::mat4 model = glm::mat4(1.0f);

    /* Loop until the user closes the window - Render Loop */
    while (!glfwWindowShouldClose(window))
    {
        /* User inputs */
        processInput(window);
        
        /* Camera Calculations */
        glm::mat4 projection = glm::perspective(glm::radians(camera.fov),
                                                (float)windowWidth / (float)windowHeight,
                                                0.1f, 100.0f);
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 mvp = projection * view * model;
        glm::mat4 modelIT = glm::transpose(glm::inverse(model));

        /* Rendering */
        // glClearColor(0.1f,0.4f,0.6f, 1.0f);
        glClearColor(0.1f,0.1f,0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Draw Objects */
        basicMaterialShader.use();

        int i = glm::sin(glfwGetTime() * 0.2f + 4.7f) * 12 + 12;
        std::cout<< i << std::endl;
        basicMaterialShader.setUniformFloat3("u_material.ambient", materials[i].ambient);
        basicMaterialShader.setUniformFloat3("u_material.diffuse", materials[i].diffuse);
        basicMaterialShader.setUniformFloat3("u_material.specular", materials[i].specular);
        basicMaterialShader.setUniformFloat("u_material.shininess", materials[i].shininess);

        basicMaterialShader.setUniformMat4("u_mvp", mvp);
        basicMaterialShader.setUniformMat4("u_model", model);
        basicMaterialShader.setUniformMat4("u_modelIT", modelIT);
        basicMaterialShader.setUniformFloat3("u_viewPos", camera.position);
        
        cube.draw();

        /* Draw Light */
        lightSourceShader.use();
        glm::mat4 lightModel = glm::translate(model, lightPos);
        lightModel = glm::scale(lightModel, glm::vec3(0.2f));
        mvp = projection * view * lightModel;
        lightSourceShader.setUniformMat4("u_mvp", mvp);
        lightCube.draw();

        /* ImGUI */
        // feed inputs to dear imgui, start new frame
        // ImGui_ImplOpenGL3_NewFrame();
        // ImGui_ImplGlfw_NewFrame();
        // ImGui::NewFrame();
        // ImGui::Begin("Demo window");
        // ImGui::End();

        // ImGui::Render();
        // ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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