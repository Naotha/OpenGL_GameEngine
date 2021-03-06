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
#include "GameComponent/LightRenderer.hpp"

class MyApplication : public Application{
public:
    float lastX;
    float lastY;
    bool firstMouse;
    bool cameraMode;
    Camera camera;

    bool spotLightOn;
    bool shibaMove;
    SpotLightRenderer* spotLightRenderer;

    Scene scene;
    Shader shader;
    FBO sceneFBO;

    bool guiOn = false;
    EditorGui* editorGui;
    SceneWidget* sceneWindow;
    ParametersWidget* parametersWindow;

    FrameBufferSizeEventCall* frameBufferSizeEvent;
    CursorPosEventCall* cursorPosEvent;
    MouseButtonEventCall* mouseButtonEvent;
    MouseScrollEventCall* mouseScrollEvent;
    KeyEventCall* keyEvent;
    KeyEventCall* guiKeyEvent;

    std::vector<GameObject*> shibas;

    MyApplication(Window& window) : Application(window), camera(glm::vec3(0.0f, 0.0f, 3.0f)),
    shader("./resources/shaders/standardShader.vert", "./resources/shaders/standardShader.frag")
    {
        lastX = (float)window.GetWidth() / 2;
        lastY = (float)window.GetHeight() / 2;
        firstMouse = true;
        cameraMode = false;
        spotLightOn = false;
        shibaMove = true;
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
            renderer->GetGBuffer().resize(window.GetWidth(), window.GetHeight());
        }
    }

    void processInput(GLFWwindow* glfwWindow)
    {
        if (glfwGetKey(glfwWindow, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(glfwWindow, true);

        if (cameraMode)
        {
            float deltaTime = renderer->GetDeltaTime();
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
            if (spotLightOn)
            {
                spotLightRenderer->Enable();
            }
            else
            {
                spotLightRenderer->Disable();
            }
        }
        if (key == GLFW_KEY_Q && action == GLFW_PRESS)
        {
            shibaMove = !shibaMove;
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
                renderer->GetGBuffer().resize(window.GetWidth(), window.GetHeight());
                std::cout<<window.GetWidth()<<" "<<window.GetHeight()<<std::endl;
                sceneWindow->Disable();
            }
        }
    }

    void scroll_callback(GLFWwindow* glfwWindow, double xOffset, double yOffset)
    {
        camera.processMouseScroll(yOffset);
    }

    void RegisterEvents()
    {
        EventHandler::SetFrameBufferSizeCallback(this, &MyApplication::framebuffer_size_callback);
        EventHandler::SetCursorPosCallback(this, &MyApplication::mouse_callback);
        EventHandler::SetMouseButtonCallback(this, &MyApplication::mouse_button_callback);
        EventHandler::SetMouseScrollCallback(this, &MyApplication::scroll_callback);
        EventHandler::SetKeyCallback(this, &MyApplication::key_callback);
        EventHandler::SetKeyCallback(this, &MyApplication::gui_key_callback);
    }

    void Setup() override
    {
        RegisterEvents();

        /// Lights
        glm::vec3 lightPos = glm::vec3(100.0f, 100.0f, 100.0f);
        glm::vec3 lightCol = glm::vec3 (1.0f, 1.0f, 1.0f);
        glm::vec3 ambientCol = lightCol * 0.2f;
        glm::vec3 diffuseCol = lightCol * 0.8f;
        DirectionalLight dirLight = DirectionalLight(-lightPos, ambientCol, diffuseCol, lightCol);
        SpotLight spotLight = SpotLight(camera.position, camera.getFront(), glm::cos(glm::radians(12.5f)), glm::cos(glm::radians(17.5f)), CONST_ATTENUATION, ambientCol, diffuseCol, lightCol);

        /// GameObjects
        Model* shibaModel = ModelLoader::LoadModel("./resources/models/shiba/scene.gltf");
        for (int i = 0; i < 30; i++)
        {
            GameObject* shiba = scene.CreateGameObject();
            ModelRenderer* modelRenderer = new ModelRenderer(shibaModel, shader);
            shiba->AddComponent(modelRenderer);
            shiba->SetRotation(glm::quat(glm::vec3(0.0f, 0.0f, glm::radians(90.0f))));
            shiba->SetPosition(glm::vec3( -50.0f + i * 3.0f, 0.0f, 0.0f));
            shibas.push_back(shiba);
        }

        GameObject* baseTerrain = scene.CreateGameObject();
        Model* baseTerrainModel = ModelLoader::LoadModel("./resources/models/base_terrain/base_terrain.obj");
        ModelRenderer* baseTerrainModelRenderer = new ModelRenderer(baseTerrainModel, shader);
        baseTerrain->AddComponent(baseTerrainModelRenderer);
        baseTerrain->SetPosition(glm::vec3(0.0f, -1.0f, 0.0f));
        
        std::vector<glm::vec3> treePositions = {
            glm::vec3(7.0f, -1.0f,  20.0f),
            glm::vec3(23.0f, -1.0f, -40.0f),
            glm::vec3(-40.0f, -1.0f, -50.0f),
            glm::vec3(0.0f, -1.0f, 30.0f),
            glm::vec3(-7.0f, -1.0f,  -20.0f),
            glm::vec3(-23.0f, -1.0f, 40.0f),
            glm::vec3(40.0f, -1.0f, 50.0f),
            glm::vec3(20.0f,  -1.0f, 7.0f),
            glm::vec3(-40.0f,  -1.0f, 23.0f),
            glm::vec3(-50.0f,  -1.0f, -40.0f),
            glm::vec3(30.0f,  -1.0f, 0.0f),
            glm::vec3(-20.0f,  -1.0f, -7.0f),
            glm::vec3(40.0f,  -1.0f, - 23.0f),
            glm::vec3(50.0f,  -1.0f, 40.0f),
        };

        Model* treeModel = ModelLoader::LoadModel("./resources/models/tree/tree.obj");
        ModelRenderer* treeModelRenderer = new ModelRenderer(treeModel, shader);
        for (auto& position : treePositions)
        {
            GameObject* tree = scene.CreateGameObject();
            tree->AddComponent(treeModelRenderer);
            tree->SetPosition(position);
        }
        
        GameObject* directionalLight = scene.CreateGameObject();
        GameObject* spotLightObject = scene.CreateGameObject();
        DirectionalLightRenderer* dirLightRenderer = new DirectionalLightRenderer(dirLight, shader);
        spotLightRenderer = new SpotLightRenderer(spotLight, shader);
        directionalLight->AddComponent(dirLightRenderer);
        spotLightObject->AddComponent(spotLightRenderer);
        spotLightRenderer->Disable();

        /// Renderer
        glm::vec2 viewPortSize = glm::vec2(window.GetWidth(), window.GetHeight());
        renderer->Init(&camera, &scene, viewPortSize);

        /// SETUP IMGUI
        editorGui = new EditorGui(window);
        sceneWindow = new SceneWidget(sceneFBO, baseTerrain, renderer->GetView(), renderer->GetProjection());
        parametersWindow = new ParametersWidget(sceneWindow, baseTerrain);
        editorGui->AddWidget(sceneWindow);
        editorGui->AddWidget(parametersWindow);
    }

    void PreLoop() override
    {
        /// PRE RENDER
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
        processInput(window.GetGLFWWindow());
        renderer->PreRender();
    }

    void OnLoop() override
    {
        if (shibaMove)
        {
            for (int i = 0; i < shibas.size(); i++)
            {
                glm::mat4 sinTranslation = glm::mat4(1.0f);
                sinTranslation = glm::translate(sinTranslation, glm::vec3(0.0f, 0.0f, glm::sin(float(glfwGetTime()) - float(i)) * 10.0f));
                shibas[i]->transform.CalculateModelMatrix(sinTranslation);
            }
        }

        /// LOGIC
        if (spotLightOn)
        {
            spotLightRenderer->GetSpotLight().setPosition(camera.position);
            spotLightRenderer->GetSpotLight().setDirection(camera.getFront());
        }
    };

    void PostLoop() override
    {
        /// POST RENDER
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

        std::cout<<1.0f / renderer->GetDeltaTime()<<std::endl;
    }
};

int main()
{
    Window window("OpenGL Engine", 1280, 720);
    MyApplication myApp(window);
    myApp.Run();
    return 0;
}
