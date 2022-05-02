#ifndef OPENGL_GAMEENGINE_SHADOWMAP_HPP
#define OPENGL_GAMEENGINE_SHADOWMAP_HPP

#include <glad/glad.h>
#include <vector>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Engine/shader.hpp"

class ShadowMap
{
public:
    unsigned int ID;
    unsigned int depthMap;

    ShadowMap() : ShadowMap(1024, 1024) {};

    ShadowMap(int width, int height)
    {
        size = glm::vec2(width, height);
        glGenFramebuffers(1, &ID);
        glBindFramebuffer(GL_FRAMEBUFFER, ID);

        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void resize(int width, int height)
    {
        size = glm::vec2(width, height);
        glBindFramebuffer(GL_FRAMEBUFFER, ID);

        glGenTextures(1, &depthMap);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        glBindFramebuffer(GL_FRAMEBUFFER,0);
    }

    glm::vec2 GetSize()
    {
        return size;
    }

    void SetShadowUniforms(Shader shader)
    {
        shader.bind();
        shader.setUniformMat4("u_lightSpaceMatrix", lightSpaceMatrix);
        shader.unbind();
    }

    void SetShadowMapInShader(Shader shader)
    {
        shader.bind();
        glActiveTexture(GL_TEXTURE0);
        shader.setUniformInt("depthMap", 0);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        shader.unbind();
    }

    void bind() { glBindFramebuffer(GL_FRAMEBUFFER, ID); }
    void unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
    void deleteShadowMap() { glDeleteFramebuffers(1, &ID); }

private:
    glm::vec2 size;

    float nearPlane = 1.0f;
    float farPlane = 100.0f;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, nearPlane, farPlane);
    glm::mat4 lightView = glm::lookAt(glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
};

#endif //OPENGL_GAMEENGINE_SHADOWMAP_HPP
