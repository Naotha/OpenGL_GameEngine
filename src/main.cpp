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


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
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

/* ImGUI */
bool resized = true;
bool translate = false;
bool rotate = false;
bool scale = false;

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
    /* Initialize the library */
    if (!glfwInit())
        return false;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL Engine", nullptr, nullptr);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Set callbacks */
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback); // Change viewport when window is resized
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    /// SETUP IMGUI
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::FileBrowser fileDialog;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

    // Setup Dear ImGui style
    auto& colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.1f, 0.1f, 1.0f };
    colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f };
    colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.3f, 0.3f, 1.0f };
    colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };
    colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f };
    colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.3f, 0.3f, 1.0f };
    colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };
    colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f };
    colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.3f, 0.3f, 1.0f };
    colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };
    colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };
    colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.38f, 0.38f, 1.0f };
    colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.28f, 0.28f, 1.0f };
    colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.2f, 0.2f, 1.0f };
    colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };
    colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.15f, 0.15f, 1.0f };

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();

    fileDialog.SetTitle("Load Model...");
    fileDialog.SetTypeFilters({ ".gltf", ".obj" });
    
    /// SETUP OPENGL
    /* Initialize GLAD */
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return false;
    }

    /* GL Enable */
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    /* Set Viewport */
    glViewport(0, 0, windowWidth, windowHeight);
    
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
    glm::mat4 model = glm::mat4(1.0f);

    /* Init FrameRate Calculation */
    float currentFrame = 0;
    float lastFrame = currentFrame;
    float frameTime = 0;
    int fps = 0;

    // Framebuffer
    FBO fbo(windowWidth, windowHeight);
    float prevViewPanelX = (float)windowWidth;
    float prevViewPanelY = (float)windowHeight;

    /* Loop until the user closes the window - Render Loop */
    while (!glfwWindowShouldClose(window))
    {
        /* Poll for and process events */
        processInput(window);
        glfwPollEvents();

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

        /* ImGUI PRE RENDER */
        // Feed inputs to dear ImGui, start new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus |
        ImGuiWindowFlags_NoBackground;

        ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImGui::SetNextWindowPos(viewport->Pos);
        ImGui::SetNextWindowSize(viewport->Size);
        ImGui::SetNextWindowViewport(viewport->ID);
        ImGui::Begin("InvisibleDockSpace", nullptr, windowFlags);

        ImGuiID dockSpaceId = ImGui::GetID("InvisibleWindowDockSpace");

        ImGui::DockSpace(dockSpaceId, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
        ImGui::End();
        ImGui::Begin("Render");
        ImGuizmo::BeginFrame();
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();

        if ((viewportPanelSize.x < prevViewPanelX || viewportPanelSize.y < prevViewPanelY) ||
            (viewportPanelSize.x > prevViewPanelX || viewportPanelSize.y > prevViewPanelY))
        {
            std::cout << "I am resized: X-" << viewportPanelSize.x << " Y-" << viewportPanelSize.y << std::endl;
            fbo.resize(viewportPanelSize.x, viewportPanelSize.y);
            resized = false;
            prevViewPanelX = viewportPanelSize.x;
            prevViewPanelY = viewportPanelSize.y;
        }

        /// RENDER
        glViewport(0, 0, viewportPanelSize.x, viewportPanelSize.y);
        fbo.bind();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        /* Draw Objects */

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
        fbo.unbind();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        /* Camera Calculations */
        glm::mat4 projection = glm::perspective(glm::radians(camera.fov),
                                                viewportPanelSize.x / viewportPanelSize.y,
                                                0.1f, 100.0f);
        glm::mat4 view = camera.getViewMatrix();
        glm::mat4 mvp;
        shader.bind();
        mvp = projection * view * model;
        glm::mat4 modelIT = glm::transpose(glm::inverse(model));
        shader.setUniformMat4("u_mvp", mvp);
        shader.setUniformMat4("u_model", model);
        shader.setUniformMat4("u_modelIT", modelIT);
        shader.setUniformFloat3("u_viewPos", camera.position);

        /* ImGUI RENDER */
        ImGuizmo::SetOrthographic(false);
        ImGuizmo::SetDrawlist();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, ImGui::GetWindowWidth(), ImGui::GetWindowHeight());

        ImGui::GetWindowDrawList()->AddImage((void *)(fbo.texture),
                     ImVec2(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y),
                     ImVec2(ImGui::GetWindowPos().x + ImGui::GetWindowWidth(),ImGui::GetWindowPos().y + ImGui::GetWindowHeight()),
                     ImVec2(0, 1), ImVec2(1, 0));
        if (translate)
        {
            ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection),
                                ImGuizmo::OPERATION::TRANSLATE, ImGuizmo::LOCAL, glm::value_ptr(model));
        }
        else if (rotate)
        {
            ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection),
                                ImGuizmo::OPERATION::ROTATE, ImGuizmo::LOCAL, glm::value_ptr(model));
        }
        else if (scale)
        {
            ImGuizmo::Manipulate(glm::value_ptr(view), glm::value_ptr(projection),
                                ImGuizmo::OPERATION::SCALE, ImGuizmo::LOCAL, glm::value_ptr(model));
        }
        ImGui::End();

        ImGui::Begin("Parameters");
        if (ImGui::Button("Translate"))
        {
            translate = !translate;
            rotate = false;
            scale = false;
        }
        if (ImGui::Button("Rotate"))
        {
            translate = false;
            rotate = !rotate;
            scale = false;
        }
        if (ImGui::Button("Scale"))
        {
            translate = false;
            rotate = false;
            scale = !scale;
        }
        if (ImGui::Button("Reset"))
        {
            model = glm::mat4(1.0f);
        }

        if (ImGui::Button("Load Model"))
        {
            fileDialog.Open();
        }
        fileDialog.Display();

        std::string newModelPath;
        if (fileDialog.HasSelected())
        {
            newModelPath = fileDialog.GetSelected().string();
            fileDialog.ClearSelected();
        }
        if (!newModelPath.empty())
        {
            std::cout<<newModelPath<<std::endl;
            delete(testModel);
            std::replace( newModelPath.begin(), newModelPath.end(), '\\', '/');
            testModel = ModelLoader::LoadModel(newModelPath);
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        /// POST RENDER
        /* Swap front and back buffers */
        glfwSwapBuffers(window);
        /* Calculate time between frames */
        calculateDeltaTime();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    resized = true;
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
        if (!cameraMode)
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