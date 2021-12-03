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
#include "Engine/camera.hpp"
#include "Engine/material.h"
#include "Engine/light.hpp"
#include "Engine/model.hpp"


void frambuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
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

/* Light */
bool spotLightOn = true;

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
    glfwSetKeyCallback(window, key_callback);
    
    /* Initialize Shaders */
    Shader lightingShader("./shaders/lightingShader.vert", "./shaders/lightingShader.frag");
    Shader lightSourceShader("./shaders/lightSourceShader.vert", "./shaders/lightSourceShader.frag");
    
    /* Create Objects */
    Texture diffuseMap("./textures/container.png", TextureType::DIFFUSE);
    Texture specularMap("./textures/container_specular.png", TextureType::SPECULAR);
    std::vector<Texture> cubeTextures = {diffuseMap, specularMap};
    Mesh cube(cubeVertices, cubeIndices, cubeTextures);

    Mesh lightCube(cubeVertices, cubeIndices);

    //Model nano("./models/nanosuit/nanosuit.obj");
    Model backpack("./models/backpack/backpack.obj");
    /* Light */
    lightingShader.bind();
    lightingShader.setUniformFloat("u_material.shininess", 32.0f);
    glm::vec3 lightPos(1.0f, 1.0f, 1.0f);
    glm::vec3 lightCol(1.0f, 1.0f, 1.0f);
    glm::vec3 ambientCol = lightCol * glm::vec3(0.2f);
    glm::vec3 diffuseCol = lightCol * glm::vec3(0.8f);
    lightSourceShader.bind();
    lightSourceShader.setUniformFloat3("u_lightCol", lightCol);

    /* Transformation Matrices */
    glm::mat4 model = glm::mat4(1.0f);

    // Light Types
    DirectionalLight dirLight(-lightPos, ambientCol, diffuseCol, lightCol);
    dirLight.setLightInShader("u_dirLight", lightingShader);

    SpotLight spotLight(camera.position, camera.getFront(), glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f)), CONST_ATTENUATION, ambientCol, diffuseCol, lightCol);
    spotLight.setLightInShader("u_spotLights[0]", lightingShader);
    lightingShader.bind();
    lightingShader.setUniformInt("u_spotLightsNum", 1);

    std::vector<glm::vec3> cubePositions{
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };

    std::vector<glm::vec3> pointLightPositions{
        glm::vec3( 0.0f,  0.0f, -3.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f)
    };

    PointLight pointLight1(pointLightPositions[0], CONST_ATTENUATION, ambientCol, diffuseCol, lightCol);
    pointLight1.setLightInShader("u_pointLights[0]", lightingShader);
    PointLight pointLight2(pointLightPositions[1], CONST_ATTENUATION, ambientCol, diffuseCol, lightCol);
    pointLight2.setLightInShader("u_pointLights[1]", lightingShader);
    lightingShader.bind();
    lightingShader.setUniformInt("u_pointLightsNum", 2);

    double currentFrame = 0;
    double lastFrame = currentFrame;
    double deltaTime;
    double frameTime = 0;
    int fps = 0;

    /* Loop until the user closes the window - Render Loop */
    while (!glfwWindowShouldClose(window))
    {
        /* User inputs */
        processInput(window);

        // FPS counter
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        frameTime += deltaTime;
        fps++;
        if (frameTime > 1)
        {
            std::cout << fps << std::endl;
            fps = 0;
            frameTime = 0;
        }
        
        /* Camera Calculations */
        glm::mat4 projection = glm::perspective(glm::radians(camera.fov),
                                                (float)windowWidth / (float)windowHeight,
                                                0.1f, 100.0f);
        glm::mat4 view = camera.getViewMatrix();
        

        /* Rendering */
        // glClearColor(0.1f,0.4f,0.6f, 1.0f);
        glClearColor(0.1f,0.1f,0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Draw Objects */
        glm::mat4 mvp;
        lightingShader.bind();
        glm::mat4 model_i = glm::translate(model, cubePositions[0]);
        mvp = projection * view * model_i;
        glm::mat4 modelIT = glm::transpose(glm::inverse(model_i));
        lightingShader.setUniformMat4("u_mvp", mvp);
        lightingShader.setUniformMat4("u_model", model_i);
        lightingShader.setUniformMat4("u_modelIT", modelIT);
        lightingShader.setUniformFloat3("u_viewPos", camera.position);
        backpack.draw(lightingShader);
        //nano.draw(lightingShader);
        // glm::mat4 mvp;
        // for (int i = 0; i < cubePositions.size(); i++)
        // {
        //     lightingShader.bind();

        //     glm::mat4 model_i = glm::translate(model, cubePositions[i]);
        //     mvp = projection * view * model_i;
        //     glm::mat4 modelIT = glm::transpose(glm::inverse(model_i));

        //     lightingShader.setUniformMat4("u_mvp", mvp);
        //     lightingShader.setUniformMat4("u_model", model_i);
        //     lightingShader.setUniformMat4("u_modelIT", modelIT);
        //     lightingShader.setUniformFloat3("u_viewPos", camera.position);
            
        //     //cube.draw(lightingShader);
        //     backpack.draw(lightingShader);
        // }

        lightingShader.bind();
        spotLight.setPosition(camera.position);
        spotLight.setDirection(camera.getFront());
        //std::cout << camera.getFront().x << camera.getFront().y << camera.getFront().z << std::endl;
        spotLight.setLightInShader("u_spotLights[0]", lightingShader);
        //lightingShader.setUniformFloat3("u_spotLights[0].position", camera.position);


        // /* Draw Light */
        // for (int i = 0; i < pointLightPositions.size(); i++)
        // {
        //     lightSourceShader.bind();
        //     glm::mat4 lightModel = glm::translate(model, pointLightPositions[i]);
        //     lightModel = glm::scale(lightModel, glm::vec3(0.2f));
        //     mvp = projection * view * lightModel;
        //     lightSourceShader.setUniformMat4("u_mvp", mvp);
        //     lightCube.draw(lightSourceShader);
        // }

        if (spotLightOn)
        {
            lightingShader.bind();
            lightingShader.setUniformInt("u_spotLightsNum", 1);
        }
        else
        {
            lightingShader.bind();
            lightingShader.setUniformInt("u_spotLightsNum", 0);
        } 

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

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
    {
        spotLightOn = !spotLightOn;
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