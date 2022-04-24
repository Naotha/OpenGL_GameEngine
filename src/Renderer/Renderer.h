#ifndef OPENGL_GAMEENGINE_RENDERER_H
#define OPENGL_GAMEENGINE_RENDERER_H

#include "GameObject/Scene.hpp"
#include "Engine/FBO.hpp"
#include "Engine/camera.hpp"
#include "Engine/shader.hpp"
#include "Window/Window.h"

class Renderer {
public:
    static Renderer* GetInstance();
    void SetViewportSize(glm::vec2& viewportSize);

    void Init(Camera* mainCamera, Scene* mainScene, glm::vec2& viewPortSize);
    void PreRender();
    void Render();
    void PostRender();

    void DrawToWindow(Window& window);

    FBO& GetFBO() { return mainFBO; }
    glm::mat4& GetView() { return view; }
    glm::mat4& GetProjection() { return projection; }

    void AddPointLight() { pointLightCount++; }
    void AddSpotLight() { spotLightCount++; }

    int GetPointLightCount() { return pointLightCount; }
    int GetSpotLightCount() { return spotLightCount; }

    void AddShader(Shader* shader)
    {
        int i = 0;
        while (i < activeShaders.size() && activeShaders[i]->getID() != shader->getID()){
            i++;
        }

        if (i >= activeShaders.size())
        {
            activeShaders.push_back(shader);
        }
    }

    float GetDeltaTime() { return deltaTime; }

private:
    Renderer();

    void calculateDeltaTime()
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrameTime;
        lastFrameTime = currentFrame;
    }

    static Renderer* instance;
    glm::vec2 viewportSize;
    FBO mainFBO;

    glm::mat4 view;
    glm::mat4 projection;
    int pointLightCount = 0;
    int spotLightCount = 0;

    Camera* mainCamera;
    Scene* mainScene;

    std::vector<Shader*> activeShaders;

    float deltaTime = 0.0f;
    float lastFrameTime = 0.0f;
};

Renderer* Renderer::instance = nullptr;

Renderer* Renderer::GetInstance() {
    if (instance == nullptr)
    {
        instance = new Renderer();
    }
    return instance;
}

void Renderer::Init(Camera* mainCamera, Scene* mainScene, glm::vec2& viewportSize)
{
    this->mainCamera = mainCamera;
    this->mainScene = mainScene;
    this->viewportSize = viewportSize;
    projection = glm::perspective(glm::radians(mainCamera->fov),
                                  viewportSize.x / viewportSize.y,
                                  0.1f, 1000.0f);
    view = mainCamera->getViewMatrix();
}

void Renderer::PreRender() {
    mainFBO.bind();
    glViewport(0, 0, viewportSize.x, viewportSize.y);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Camera Calculations */
    projection = glm::perspective(glm::radians(mainCamera->fov),
                                  viewportSize.x / viewportSize.y,
                                  0.1f, 1000.0f);
    view = mainCamera->getViewMatrix();
    glm::mat4 vp;
    vp = projection * view;

    for (Shader* shader : activeShaders)
    {
        shader->bind();
        shader->setUniformMat4("u_vp", vp);
        shader->setUniformFloat3("u_viewPos", mainCamera->position);
        shader->setUniformInt("u_pointLightsNum", pointLightCount);
        shader->setUniformInt("u_spotLightsNum", spotLightCount);
        shader->unbind();
    }
}

void Renderer::Render()
{
    mainScene->Update();
    mainScene->Render();
}

void Renderer::PostRender() {
    mainFBO.unbind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    calculateDeltaTime();
}

void Renderer::DrawToWindow(Window& window) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mainFBO.ID);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, viewportSize.x, viewportSize.y, 0, 0, window.GetWidth(), window.GetHeight(),
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void Renderer::SetViewportSize(glm::vec2& viewportSize) {
    this->viewportSize = viewportSize;
}

Renderer::Renderer() : mainFBO(1280, 720){};

#endif //OPENGL_GAMEENGINE_RENDERER_H
