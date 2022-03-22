#include "Renderer.h"

Renderer* Renderer::GetInstance() {
    if (instance == nullptr)
    {
        instance = new Renderer();
    }
    return instance;
}

void Renderer::Init(Camera& camera)
{
    projection = glm::perspective(glm::radians(camera.fov),
                                  viewportSize.x / viewportSize.y,
                                  0.1f, 10000.0f);
    view = camera.getViewMatrix();
}

void Renderer::PreRender(Camera& camera, Shader& shader) {
    mainFBO.bind();
    glViewport(0, 0, viewportSize.x, viewportSize.y);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* Camera Calculations */
    projection = glm::perspective(glm::radians(camera.fov),
                                  viewportSize.x / viewportSize.y,
                                  0.1f, 10000.0f);
    view = camera.getViewMatrix();
    glm::mat4 vp;
    shader.bind();
    vp = projection * view;
    shader.setUniformMat4("u_vp", vp);
    shader.setUniformFloat3("u_viewPos", camera.position);
    shader.unbind();
}

void Renderer::Render(Scene& scene) {
    scene.Update();
    scene.Render();
}

void Renderer::PostRender() {
    mainFBO.unbind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Renderer::DrawToWindow(Window& window) {
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mainFBO.ID);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glBlitFramebuffer(0, 0, viewportSize.x, viewportSize.y, 0, 0, window.GetWidth(), window.GetHeight(),
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void Renderer::SetViewportSize(glm::vec2 viewportSize) {
    this->viewportSize = viewportSize;
}

Renderer::Renderer() : mainFBO(), viewportSize() {};

