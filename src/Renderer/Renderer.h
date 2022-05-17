#ifndef OPENGL_GAMEENGINE_RENDERER_H
#define OPENGL_GAMEENGINE_RENDERER_H

#include "GameObject/Scene.hpp"
#include "Engine/FBO.hpp"
#include "Engine/GBuffer.hpp"
#include "Engine/ShadowMap.hpp"
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
    GBuffer& GetGBuffer() { return gBuffer; }
    glm::mat4& GetView() { return view; }
    glm::mat4& GetProjection() { return projection; }
    float GetDeltaTime() { return deltaTime; }

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

    void AddShadowMap(DirectionalLight& light,  float projectionSize, float nearPlane = 0.1f, float farPlane = 300.0f)
    {
        ShadowMap shadowMap(2048, 2048, -light.getDirection(), projectionSize, nearPlane, farPlane);
        shadowMaps.push_back(shadowMap);
    }

private:
    Renderer();

    void calculateDeltaTime()
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrameTime;
        lastFrameTime = currentFrame;
    }

    void InitScreenQuad()
    {
        screenQuadVAO.bind();
        screenQuadVBO.SetData(screenQuadVertices);
        screenQuadVAO.linkAttribPointer(screenQuadVBO, 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        screenQuadVAO.linkAttribPointer(screenQuadVBO, 1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        screenQuadVAO.linkAttribPointer(screenQuadVBO, 2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
        screenQuadVAO.unbind();
    }

    void RenderScreenQuad(Shader shader)
    {
        shader.bind();
        screenQuadVAO.bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        screenQuadVAO.unbind();
        shader.unbind();
    }

    static Renderer* instance;
    glm::vec2 viewportSize;
    FBO mainFBO;
    GBuffer gBuffer;
    VAO screenQuadVAO;
    VBO screenQuadVBO;
    std::vector<Vertex> screenQuadVertices = {
            {{-1.0f,  1.0f, 0.0f}, {0.0f,  0.0f, 0.0f}, {0.0f, 1.0f}},
            {{-1.0f, -1.0f, 0.0f}, {0.0f,  0.0f, 0.0f}, {0.0f, 0.0f}},
            {{1.0f,  1.0f, 0.0f}, {0.0f,  0.0f, 0.0f}, {1.0f, 1.0f}},
            {{1.0f, -1.0f, 0.0f}, {0.0f,  0.0f, 0.0f}, {1.0f, 0.0f}}
    };

    glm::mat4 view;
    glm::mat4 projection;
    int pointLightCount = 0;
    int spotLightCount = 0;

    Camera* mainCamera;
    Scene* mainScene;

    Shader defaultGeometryPassShader;
    Shader defaultLightingPassShader;
    Shader shadowShader;
    Shader shadowTest;
    std::vector<Shader*> activeShaders = {&defaultLightingPassShader, &defaultGeometryPassShader, &shadowShader, &shadowTest};
    std::vector<ShadowMap> shadowMaps = {};

    float deltaTime = 0.0f;
    float lastFrameTime = 0.0f;

    bool deferredRendering = true;
    bool shadowRendering = true;
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
    mainFBO.resize(viewportSize.x, viewportSize.y);
    gBuffer.resize(viewportSize.x, viewportSize.y);
    view = mainCamera->getViewMatrix();
}

void Renderer::PreRender() {
    /* Camera Calculations */
    projection = glm::perspective(glm::radians(mainCamera->fov),
                                  viewportSize.x / viewportSize.y,
                                  0.1f, 1000.0f);
    view = mainCamera->getViewMatrix();
    glm::mat4 vp = projection * view;

    if (deferredRendering)
    {
        gBuffer.bind();
        glViewport(0, 0, viewportSize.x, viewportSize.y);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    else if (shadowRendering)
    {
//        shadowMap.bind();
//        glViewport(0, 0, shadowMap.GetSize().x, shadowMap.GetSize().y);
//
//        glClear(GL_DEPTH_BUFFER_BIT);
//        shadowMap.SetShadowUniforms(shadowShader);
    }
    else
    {
        mainFBO.bind();
        glViewport(0, 0, viewportSize.x, viewportSize.y);

        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

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

    if (deferredRendering)
    {
        gBuffer.bind();
        glViewport(0, 0, viewportSize.x, viewportSize.y);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        mainScene->RenderWithShader(defaultGeometryPassShader);

        if (shadowRendering)
        {
            for (auto& shadowMap : shadowMaps)
            {
                shadowMap.bind();
                glViewport(0, 0, shadowMap.GetSize().x, shadowMap.GetSize().y);

                glClear(GL_DEPTH_BUFFER_BIT);
                glCullFace(GL_FRONT);
                shadowMap.SetShadowUniforms(shadowShader);
                mainScene->RenderWithShader(shadowShader);
                glCullFace(GL_BACK);
            }
        }

        mainFBO.bind();
        glViewport(0, 0, viewportSize.x, viewportSize.y);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gBuffer.BindTextures(defaultLightingPassShader);
        for (int i = 0; i < shadowMaps.size(); i++)
        {
            shadowMaps[i].SetShadowUniforms(defaultLightingPassShader, i);
            shadowMaps[i].SetShadowMapInShader(defaultLightingPassShader, i);
        }
        mainScene->RenderLightsOnly(defaultLightingPassShader);
        RenderScreenQuad(defaultLightingPassShader);
    }
    else
    {
        mainFBO.bind();
        mainScene->Render();
    }
}

void Renderer::PostRender() {
    //mainFBO.unbind();
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

Renderer::Renderer() : mainFBO(1280, 720), screenQuadVBO(screenQuadVertices),
                       defaultGeometryPassShader("./resources/shaders/geometryPassDeferred.vert", "./resources/shaders/geometryPassDeferred.frag"),
                       defaultLightingPassShader("./resources/shaders/lightingPassDeferred.vert", "./resources/shaders/lightingPassDeferred.frag"),
                       shadowShader("./resources/shaders/shadow.vert", "./resources/shaders/shadow.frag"),
                       shadowTest("./resources/shaders/lightingPassDeferred.vert", "./resources/shaders/shadowTest.frag")
{
    InitScreenQuad();
};

#endif //OPENGL_GAMEENGINE_RENDERER_H
