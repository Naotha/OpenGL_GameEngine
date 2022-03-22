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
#include "GameObject/GameObject.hpp"
#include "EventHandler/EventHandler.h"
#include "Core/Application.h"
#include "GameObject/Scene.hpp"
#include "GameObject/GameObject.hpp"
#include "GameComponent/GameComponent.hpp"
#include "GameComponent/ModelRenderer.hpp"

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
    ModelRenderer* modelRenderer;
    Scene scene;
    GameObject* parentShiba;
    GameObject* childShiba;

    glm::vec3 lightPos;
    glm::vec3 lightCol;
    glm::vec3 ambientCol;
    glm::vec3 diffuseCol;
    DirectionalLight dirLight;
    SpotLight spotLight;

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
        sceneFBO = renderer->GetFBO();
        sceneFBO.resize(window.GetWidth(), window.GetHeight());
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

    void RegisterEvents()
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
    }

    void Setup() override
    {
        RegisterEvents();

        testModel = ModelLoader::LoadModel("./resources/models/shiba/scene.gltf");
        modelRenderer = new ModelRenderer(testModel, shader);

        parentShiba = scene.CreateGameObject();
        childShiba = scene.CreateGameObject();
        childShiba->SetPosition(glm::vec3(1.0f, 0.0f, 0.0f));
        childShiba->SetScale(glm::vec3(0.5f, 0.5f, 0.5f));

        parentShiba->AddComponent(modelRenderer);
        childShiba->AddComponent(modelRenderer);

        parentShiba->AddChild(childShiba);

        /* Light */
        shader.bind();
        shader.setUniformFloat("u_material.shininess", 32.0f);
        shader.unbind();

        // Light Types
        lightPos = glm::vec3(1.0f, 1.0f, 1.0f);
        lightCol = glm::vec3 (1.0f, 1.0f, 1.0f);
        ambientCol = lightCol * 0.2f;
        diffuseCol = lightCol * 0.8f;
        dirLight = DirectionalLight(-lightPos, ambientCol, diffuseCol, lightCol);
        dirLight.setLightInShader("u_dirLight", shader);

        spotLight = SpotLight(camera.position, camera.getFront(), glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f)), CONST_ATTENUATION, ambientCol, diffuseCol, lightCol);
        spotLight.setLightInShader("u_spotLights[0]", shader);
        shader.bind();
        shader.setUniformInt("u_spotLightsNum", 1);
        shader.unbind();

        // Framebuffer
        glm::vec2 viewPortSize = glm::vec2(window.GetWidth(), window.GetHeight());
        renderer->Init(camera, viewPortSize);

        /// SETUP IMGUI
        editorGui = new EditorGui(window);
        sceneWindow = new SceneWidget(sceneFBO, parentShiba, renderer->GetView(), renderer->GetProjection());
        parametersWindow = new ParametersWidget(sceneWindow, parentShiba);
        editorGui->AddWidget(sceneWindow);
        editorGui->AddWidget(parametersWindow);
    }

    void OnLoop() override
    {
        /// PRE-RENDER
        if (guiOn)
        {
            glm::vec2 viewport = glm::vec2(sceneWindow->GetSize().x, sceneWindow->GetSize().y);
            if (viewport.x != 0 && viewport.y != 0)
                renderer->SetViewportSize(viewport);
        }
        else
        {
            glm::vec2 viewport = glm::vec2(window.GetWidth(), window.GetHeight());
            renderer->SetViewportSize(viewport);
        }
        renderer->PreRender(camera, shader);

        /// RENDER
        renderer->Render(scene);

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

        /// POST-RENDER
        renderer->PostRender();

        if (guiOn)
        {
            editorGui->Begin();
            editorGui->Render();
            editorGui->End();
        }
        else
        {
            renderer->DrawToWindow(window);
        }

        /// INPUT PROCESSING
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
