#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include <ImGui/imgui.h>
#include <ImGui/ImGuizmo.h>
#include <ImGui/backends/imgui_impl_glfw.h>
#include <ImGui/filebrowser/imfilebrowser.h>
#include <ImGui/backends/imgui_impl_opengl3.h>

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


void frambuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void calculateDeltaTime();

/* Window, cursor */
unsigned short windowWidth = 1000;
unsigned short windowHeight = 600;
float lastX = windowWidth / 2;
float lastY = windowHeight / 2;
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

/* Context */
GLFWwindow* window;
float prevViewPanelX = (float)windowWidth;
float prevViewPanelY = (float)windowHeight;
ImGui::FileBrowser fileDialog;

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

bool CreateGLFWContext()
{
    /// SETUP GLFW
    /* Initialize the library */
    if (!glfwInit())
        return false;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(windowWidth, windowHeight, "OpenGL Engine", NULL, NULL);
    if (!window)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return false;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    return true;
}

bool SetupOpenGL()
{
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
    glfwSetFramebufferSizeCallback(window, frambuffer_size_callback); // Change viewport when window is resized
    
    /* Set Mouse callbacks */
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);

    return true;
}

void CreateIMGUIContext()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigWindowsMoveFromTitleBarOnly=true;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
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

    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
      style.WindowRounding = 0.0f;
      style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }
    // Setup Platform/Renderer bindings
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init();
    prevViewPanelX = windowWidth;
    prevViewPanelY = windowHeight;

    fileDialog.SetTitle("Load Model...");
    fileDialog.SetTypeFilters({ ".gltf", ".obj" });
}

int main(void)
{
    /// SETUP GLFW
    if(!CreateGLFWContext())
        return -1;

    /// SETUP IMGUI
    CreateIMGUIContext();
    
    /// SETUP OPENGL
    if(!SetupOpenGL())
        return -1;
    
    /* Initialize Shaders */
    Shader shader("./shaders/standardShader.vert", "./shaders/standardShader.frag");

    //Model* testModel = ModelLoader::LoadModel("./models/backpack/backpack.obj");
    Model* testModel = ModelLoader::LoadModel("./models/shiba/scene.gltf");

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
    double currentFrame = 0;
    double lastFrame = currentFrame;
    double deltaTime;
    double frameTime = 0;
    int fps = 0;

    // Framebuffer
    FBO fbo(windowWidth, windowHeight);

    /* Loop until the user closes the window - Render Loop */
    while (!glfwWindowShouldClose(window))
    {
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
        // feed inputs to dear imgui, start new frame
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
        glClearColor(0.1f,0.1f,0.1f, 1.0f);
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

        std::string newModelPath = "";
        if (fileDialog.HasSelected())
        {
            newModelPath = fileDialog.GetSelected().string();
            fileDialog.ClearSelected();
        }
        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        if (newModelPath != "")
        {
            std::cout<<newModelPath<<std::endl;
            delete(testModel);
            std::replace( newModelPath.begin(), newModelPath.end(), '\\', '/');
            testModel = ModelLoader::LoadModel(newModelPath);
        }

        /// POST RENDER
        /* Poll for and process events */
        processInput(window);
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