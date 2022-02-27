#define GLFW_INCLUDE_NONE
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <imgui/imgui.h>
#include <imguizmo/ImGuizmo.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <imgui_filebrowser/imfilebrowser.h>

#include <iostream>
#include <vector>
#include <algorithm>

#include "Engine/FBO.hpp"
#include "Engine/shader.hpp"
#include "Engine/texture.hpp"
#include "Engine/mesh.hpp"
#include "Engine/camera.hpp"
#include "Engine/material.h"
#include "Engine/light.hpp"
#include "Engine/model.hpp"
#include "Engine/modelLoader.hpp"

#include "Window/Window.h"
#include "Gui/EditorGui.h"
#include "Gui/EditorWidget.h"
#include "Gui/SceneWidget.hpp"
#include "Gui/ParametersWidget.hpp"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void calculateDeltaTime();

/* Window, cursor */
int windowWidth = 1280;
int windowHeight = 720;
float lastX = (float)windowWidth / 2;
float lastY = (float)windowHeight / 2;
bool firstMouse = true;
bool cameraMode = false;

/* Time */
float deltaTime = 0.0f;
float lastFrame = 0.0f;

/* Camera */
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

/* Light */
bool spotLightOn = false;

int main()
{
    /// SETUP GLFW
    Window window("OpenGL Engine", windowWidth, windowHeight);

    /* Set callbacks */
    glfwSetFramebufferSizeCallback(window.GetGLFWWindow(), framebuffer_size_callback); // Change viewport when window is resized
    glfwSetCursorPosCallback(window.GetGLFWWindow(), mouse_callback);
    glfwSetMouseButtonCallback(window.GetGLFWWindow(), mouse_button_callback);
    glfwSetScrollCallback(window.GetGLFWWindow(), scroll_callback);
    glfwSetKeyCallback(window.GetGLFWWindow(), key_callback);

    /* Initialize Shaders */
    Shader shader("./resources/shaders/standardShader.vert", "./resources/shaders/standardShader.frag");

    //Model* testModel = ModelLoader::LoadModel("./resources/models/backpack/backpack.obj");
    Model* testModel = ModelLoader::LoadModel("./resources/models/shiba/scene.gltf");

    /* Light */
    shader.bind();
    shader.setUniformFloat("u_material.shininess", 32.0f);
    glm::vec3 lightPos(1.0f, 1.0f, 1.0f);
    glm::vec3 lightCol(1.0f, 1.0f, 1.0f);
    glm::vec3 ambientCol = lightCol * glm::vec3(0.2f);
    glm::vec3 diffuseCol = lightCol * glm::vec3(0.8f);
    shader.unbind();

    // Light Types
    DirectionalLight dirLight(-lightPos, ambientCol, diffuseCol, lightCol);
    dirLight.setLightInShader("u_dirLight", shader);

    SpotLight spotLight(camera.position, camera.getFront(), glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f)), CONST_ATTENUATION, ambientCol, diffuseCol, lightCol);
    spotLight.setLightInShader("u_spotLights[0]", shader);
    shader.bind();
    shader.setUniformInt("u_spotLightsNum", 1);
    shader.unbind();

    /* Transformation Matrices */
    glm::vec2 viewportSize = glm::vec2(window.GetWidth(), window.GetHeight());
    glm::mat4 model = glm::mat4(1.0f);
    glm::mat4 view = camera.getViewMatrix();
    glm::mat4 projection = glm::perspective(glm::radians(camera.fov),
                                            viewportSize.x / viewportSize.y,
                                            0.1f, 100.0f);

    // Framebuffer
    FBO sceneFBO(windowWidth, windowHeight);

    /// SETUP IMGUI
    bool guiOn = true;
    EditorGui editorGui(window);
    SceneWidget* sceneWindow = new SceneWidget(sceneFBO, model, view, projection);
    ParametersWidget* parametersWindow = new ParametersWidget(sceneWindow, model);
    editorGui.AddWidget(sceneWindow);
    editorGui.AddWidget(parametersWindow);

    /* Loop until the user closes the window - Render Loop */
    while (window.IsAlive())
    {
        /// RENDER
        sceneFBO.bind();
        glViewport(0, 0, (int)sceneWindow->GetSize().x, (int)sceneWindow->GetSize().y);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Models */
        testModel->draw(shader);

        shader.bind();
        spotLight.setPosition(camera.position);
        spotLight.setDirection(camera.getFront());
        spotLight.setLightInShader("u_spotLights[0]", shader);

        if (spotLightOn)
        {
            shader.bind();
            shader.setUniformInt("u_spotLightsNum", 1);
        }
        else
        {
            shader.bind();
            shader.setUniformInt("u_spotLightsNum", 0);
        }
        sceneFBO.unbind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Camera Calculations */
        projection = glm::perspective(glm::radians(camera.fov),
                                                viewportSize.x / viewportSize.y,
                                                0.1f, 100.0f);
        view = camera.getViewMatrix();
        glm::mat4 mvp;
        shader.bind();
        mvp = projection * view * model;
        glm::mat4 modelIT = glm::transpose(glm::inverse(model));
        shader.setUniformMat4("u_mvp", mvp);
        shader.setUniformMat4("u_model", model);
        shader.setUniformMat4("u_modelIT", modelIT);
        shader.setUniformFloat3("u_viewPos", camera.position);

        if (guiOn)
        {
            editorGui.Begin();
            editorGui.Render();
            editorGui.End();

            if (parametersWindow->IsModelLoaded())
            {
                delete(testModel);
                testModel = ModelLoader::LoadModel(parametersWindow->GetModelPath());
                parametersWindow->ClearSelection();
            }
        }
        else
        {
            glBindFramebuffer(GL_READ_FRAMEBUFFER, sceneFBO.ID);
            glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
            glBlitFramebuffer(0, 0, viewportSize.x, viewportSize.y, 0, 0, window.GetWidth(), window.GetHeight(),
                              GL_COLOR_BUFFER_BIT, GL_NEAREST);
        }

        /// POST RENDER
        processInput(window.GetGLFWWindow());
        window.OnUpdate();
        calculateDeltaTime();
    }

    // Cleanup
    editorGui.Shutdown();
    window.Shutdown();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (cameraMode)
    {
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

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
    {
        cameraMode = false;
        firstMouse = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    {
        cameraMode = true;
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }
}

void mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
    if (cameraMode)
    {
        // Circumvent sudden jump on focus gain
        if (firstMouse)
        {
            lastX = xPos;
            lastY = yPos;
            firstMouse = false;
        }

        // Calculate mouse offset
        float xOffset = xPos - lastX;
        float yOffset = lastY - yPos;
        lastX = xPos;
        lastY = yPos;

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

void scroll_callback(GLFWwindow* window, double xOffset, double yOffset)
{
    camera.processMouseScroll(yOffset);
}

void calculateDeltaTime()
{
    float currentFrame = glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}