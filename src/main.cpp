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
#include "Engine/Entity.hpp"
#include "EventHandler/EventHandler.h"
#include "Core/Application.h"

class MyApplication : public Application{
public:
    float lastX;
    float lastY;
    bool firstMouse;
    bool cameraMode;
    float deltaTime;
    float lastFrame;
    Camera camera;
    bool spotLightOn;

    Shader shader;
    Model* testModel;
    Model* testModel2;
    Entity* parentShiba;
    Entity* childShiba;

    glm::vec3 lightPos;
    glm::vec3 lightCol;
    glm::vec3 ambientCol;
    glm::vec3 diffuseCol;
    DirectionalLight dirLight;
    SpotLight spotLight;

    glm::vec2 viewportSize;
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    FBO sceneFBO;

    bool guiOn = true;
    EditorGui* editorGui;
    SceneWidget* sceneWindow;
    ParametersWidget* parametersWindow;

    FrameBufferSizeEventCall* frameBufferSizeEvent;
    CursorPosEventCall* cursorPosEvent;
    MouseButtonEventCall* mouseButtonEvent;
    MouseScrollEventCall* mouseScrollEvent;
    KeyEventCall* keyEvent;
    KeyEventCall* guiKeyEvent;

    MyApplication(Window& window) : Application(window), camera(glm::vec3(0.0f, 0.0f, 3.0f)),
    shader("./resources/shaders/standardShader.vert", "./resources/shaders/standardShader.frag")
    {
        lastX = (float)window.GetWidth() / 2;
        lastY = (float)window.GetHeight() / 2;
        firstMouse = true;
        cameraMode = false;
        deltaTime = 0.0f;
        lastFrame = 0.0f;
        spotLightOn = false;
    };

    void framebuffer_size_callback(GLFWwindow* glfwWindow, int width, int height)
    {
        window.SetWidth(width);
        window.SetHeight(height);
        glViewport(0, 0, width, height);
        if (!guiOn)
        {
            sceneFBO.resize(window.GetWidth(), window.GetHeight());
        }
    }

    void processInput(GLFWwindow* glfwWindow)
    {
        if (glfwGetKey(glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(glfwWindow, true);

        if (cameraMode)
        {
            if (glfwGetKey(glfwWindow, GLFW_KEY_W) == GLFW_PRESS)
                camera.processKeyboard(FORWARD, deltaTime);
            if (glfwGetKey(glfwWindow, GLFW_KEY_S) == GLFW_PRESS)
                camera.processKeyboard(BACKWARD, deltaTime);
            if (glfwGetKey(glfwWindow, GLFW_KEY_A) == GLFW_PRESS)
                camera.processKeyboard(LEFT, deltaTime);
            if (glfwGetKey(glfwWindow, GLFW_KEY_D) == GLFW_PRESS)
                camera.processKeyboard(RIGHT, deltaTime);
        }
    }

    void mouse_button_callback(GLFWwindow* glfwWindow, int button, int action, int mods)
    {
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_RELEASE)
        {
            cameraMode = false;
            firstMouse = true;
            glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        }
        if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
        {
            cameraMode = true;
            glfwSetInputMode(glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
    }

    void mouse_callback(GLFWwindow* glfwWindow, double xPos, double yPos)
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

    void key_callback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_E && action == GLFW_PRESS)
        {
            spotLightOn = !spotLightOn;
        }
    }

    void gui_key_callback(GLFWwindow* glfwWindow, int key, int scancode, int action, int mods)
    {
        if (key == GLFW_KEY_Q && action == GLFW_PRESS && mods == GLFW_MOD_CONTROL)
        {
            guiOn = !guiOn;

            if (!guiOn)
            {
                sceneFBO.resize(window.GetWidth(), window.GetHeight());
                std::cout<<window.GetWidth()<<" "<<window.GetHeight()<<std::endl;
                sceneWindow->Disable();
            }
        }
    }

    void scroll_callback(GLFWwindow* glfwWindow, double xOffset, double yOffset)
    {
        camera.processMouseScroll(yOffset);
    }

    void calculateDeltaTime()
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
    }

    void Setup() override
    {
        frameBufferSizeEvent = new FrameBufferSizeEvent(this, &MyApplication::framebuffer_size_callback);
        cursorPosEvent = new CursorPosEvent(this, &MyApplication::mouse_callback);
        mouseButtonEvent = new MouseButtonEvent(this, &MyApplication::mouse_button_callback);
        mouseScrollEvent = new MouseScrollEvent(this, &MyApplication::scroll_callback);
        keyEvent = new KeyEvent(this, &MyApplication::key_callback);
        guiKeyEvent = new KeyEvent(this, &MyApplication::gui_key_callback);

        EventHandler::SetFrameBufferSizeCallback(frameBufferSizeEvent);
        EventHandler::SetCursorPosCallback(cursorPosEvent);
        EventHandler::SetMouseButtonCallback(mouseButtonEvent);
        EventHandler::SetMouseScrollCallback(mouseScrollEvent);
        EventHandler::SetKeyCallback(keyEvent);
        EventHandler::SetKeyCallback(guiKeyEvent);

        //Model* testModel = ModelLoader::LoadModel("./resources/models/backpack/backpack.obj");
        testModel = ModelLoader::LoadModel("./resources/models/shiba/scene.gltf");
        testModel2 = ModelLoader::LoadModel("./resources/models/shiba/scene.gltf");

        parentShiba = new Entity(testModel);
        childShiba = new Entity(testModel2);
        parentShiba->AddChild(childShiba);
        childShiba->transform.SetPosition(glm::vec3(1.0f, 0.0f, 0.0f));
        childShiba->transform.SetScale(glm::vec3(0.5f, 0.5f, 0.5f));
        parentShiba->UpdateTransform();

        /* Light */
        shader.bind();
        shader.setUniformFloat("u_material.shininess", 32.0f);
        lightPos = glm::vec3(1.0f, 1.0f, 1.0f);
        lightCol = glm::vec3 (1.0f, 1.0f, 1.0f);
        ambientCol = lightCol * glm::vec3(0.2f);
        diffuseCol = lightCol * glm::vec3(0.8f);
        shader.unbind();

        // Light Types
        dirLight = DirectionalLight(-lightPos, ambientCol, diffuseCol, lightCol);
        dirLight.setLightInShader("u_dirLight", shader);

        spotLight = SpotLight(camera.position, camera.getFront(), glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f)), CONST_ATTENUATION, ambientCol, diffuseCol, lightCol);
        spotLight.setLightInShader("u_spotLights[0]", shader);
        shader.bind();
        shader.setUniformInt("u_spotLightsNum", 1);
        shader.unbind();

        /* Transformation Matrices */
        viewportSize = glm::vec2(window.GetWidth(), window.GetHeight());
        model = glm::mat4(1.0f);
        view = camera.getViewMatrix();
        projection = glm::perspective(glm::radians(camera.fov),
                                                viewportSize.x / viewportSize.y,
                                                0.1f, 100.0f);

        // Framebuffer
        sceneFBO = FBO(window.GetWidth(), window.GetHeight());

        /// SETUP IMGUI
        editorGui = new EditorGui(window);
        sceneWindow = new SceneWidget(sceneFBO, parentShiba, view, projection);
        parametersWindow = new ParametersWidget(sceneWindow, parentShiba);
        editorGui->AddWidget(sceneWindow);
        editorGui->AddWidget(parametersWindow);
    }

    void OnLoop() override
    {
        /// RENDER
        sceneFBO.bind();
        if (guiOn)
        {
            glViewport(0, 0, (int)sceneWindow->GetSize().x, (int)sceneWindow->GetSize().y);
        }
        else
        {
            glViewport(0, 0, window.GetWidth(), window.GetHeight());
        }
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Camera Calculations */
        projection = glm::perspective(glm::radians(camera.fov),
                                      viewportSize.x / viewportSize.y,
                                      0.1f, 100.0f);
        view = camera.getViewMatrix();
        glm::mat4 vp;
        shader.bind();
        vp = projection * view;
        shader.setUniformMat4("u_vp", vp);
        shader.setUniformFloat3("u_viewPos", camera.position);
        shader.unbind();

        /* Models */
        parentShiba->Render(shader);

        shader.bind();
        spotLight.setPosition(camera.position);
        spotLight.setDirection(camera.getFront());
        spotLight.setLightInShader("u_spotLights[0]", shader);
        shader.unbind();

        if (spotLightOn)
        {
            shader.bind();
            shader.setUniformInt("u_spotLightsNum", 1);
            shader.unbind();
        }
        else
        {
            shader.bind();
            shader.setUniformInt("u_spotLightsNum", 0);
            shader.unbind();

        }
        sceneFBO.unbind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (guiOn)
        {
            editorGui->Begin();
            editorGui->Render();
            editorGui->End();

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
    };
};

int main()
{
    Window window("OpenGL Engine", 1280, 720);
    MyApplication myApp(window);
    myApp.Run();
    return 0;
}
