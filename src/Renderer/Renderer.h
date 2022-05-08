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

private:
    Renderer();

    void calculateDeltaTime()
    {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrameTime;
        lastFrameTime = currentFrame;
    }

    void InitGBufferQuad()
    {
        gBufferQuadVAO.bind();
        gBufferQuadVBO.SetData(gBufferQuadVertices);
        gBufferQuadVAO.linkAttribPointer(gBufferQuadVBO, 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        gBufferQuadVAO.linkAttribPointer(gBufferQuadVBO, 1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
        gBufferQuadVAO.linkAttribPointer(gBufferQuadVBO, 2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
        gBufferQuadVAO.unbind();
    }

    void RenderGBufferQuad(Shader shader)
    {
        shader.bind();
        gBufferQuadVAO.bind();
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        gBufferQuadVAO.unbind();
        shader.unbind();
    }

    static Renderer* instance;
    glm::vec2 viewportSize;
    FBO mainFBO;
    GBuffer gBuffer;
    VAO gBufferQuadVAO;
    VBO gBufferQuadVBO;
    std::vector<Vertex> gBufferQuadVertices = {
            {{-1.0f,  1.0f, 0.0f}, {0.0f,  0.0f, 0.0f}, {0.0f, 1.0f}},
            {{-1.0f, -1.0f, 0.0f}, {0.0f,  0.0f, 0.0f}, {0.0f, 0.0f}},
            {{1.0f,  1.0f, 0.0f}, {0.0f,  0.0f, 0.0f}, {1.0f, 1.0f}},
            {{1.0f, -1.0f, 0.0f}, {0.0f,  0.0f, 0.0f}, {1.0f, 0.0f}}
    };

    ShadowMap shadowMap;

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
    std::vector<Shader*> activeShaders = {&defaultLightingPassShader, &defaultGeometryPassShader};

    float deltaTime = 0.0f;
    float lastFrameTime = 0.0f;

    bool deferredRendering = true;
    bool shadowRendering = false;
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
        shadowMap.bind();
        glViewport(0, 0, shadowMap.GetSize().x, shadowMap.GetSize().y);

        glClear(GL_DEPTH_BUFFER_BIT);
        shadowMap.SetShadowUniforms(defaultLightingPassShader);
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
        mainScene->RenderWithShader(defaultGeometryPassShader);

        mainFBO.bind();
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        gBuffer.BindTextures(defaultLightingPassShader);
        mainScene->RenderLightsOnly(defaultLightingPassShader);

        RenderGBufferQuad(defaultLightingPassShader);
    }
    else
    {
        if (shadowRendering)
        {
            mainScene->RenderWithShader(shadowShader);
            shadowMap.unbind();

            mainFBO.bind();
            glViewport(0, 0, viewportSize.x, viewportSize.y);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            shadowMap.SetShadowMapInShader(shadowTest);
            RenderGBufferQuad(shadowTest);
        }
//        mainFBO.bind();
//        mainScene->Render();
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

Renderer::Renderer() : mainFBO(1280, 720), gBufferQuadVBO(gBufferQuadVertices),
defaultGeometryPassShader("./resources/shaders/geometryPassDeferred.vert", "./resources/shaders/geometryPassDeferred.frag"),
defaultLightingPassShader("./resources/shaders/lightingPassDeferred.vert", "./resources/shaders/lightingPassDeferred.frag"),
shadowShader("./resources/shaders/shadow.vert", "./resources/shaders/shadow.frag"),
shadowTest("./resources/shaders/lightingPassDeferred.vert", "./resources/shaders/shadowTest.frag")
{
    InitGBufferQuad();
};

#endif //OPENGL_GAMEENGINE_RENDERER_H
