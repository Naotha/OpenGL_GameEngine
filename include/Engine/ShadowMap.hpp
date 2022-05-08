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
    unsigned int shadowMap;

    ShadowMap() : ShadowMap(2048, 2048) {};

    ShadowMap(int width, int height)
    {
        lightProjection = glm::ortho(-75.0f, 75.0f, -75.0f, 75.0f, 0.1f, 300.0f);
        lightView = glm::lookAt(glm::vec3(100.0f, 100.0f, 100.0f), glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
        lightVP = lightProjection * lightView;

        size = glm::vec2(width, height);
        glGenFramebuffers(1, &ID);
        glBindFramebuffer(GL_FRAMEBUFFER, ID);

        glGenTextures(1, &shadowMap);
        glBindTexture(GL_TEXTURE_2D, shadowMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);

        auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
            std::cout<< "Shadow map error:" << fboStatus << std::endl;

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void resize(int width, int height)
    {
        size = glm::vec2(width, height);
        glBindFramebuffer(GL_FRAMEBUFFER, ID);

        glGenTextures(1, &shadowMap);
        glBindTexture(GL_TEXTURE_2D, shadowMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

        glBindFramebuffer(GL_FRAMEBUFFER,0);
    }

    glm::vec2 GetSize()
    {
        return size;
    }

    void SetShadowUniforms(Shader& shader)
    {
        shader.bind();
        shader.setUniformMat4("u_lightVP", lightVP);
        shader.unbind();
    }

    void SetShadowMapInShader(Shader& shader)
    {
        shader.bind();
        shader.setUniformInt("shadowMap", 3);
        glActiveTexture(GL_TEXTURE0 + 3);
        glBindTexture(GL_TEXTURE_2D, shadowMap);
        shader.unbind();
    }

    void bind() { glBindFramebuffer(GL_FRAMEBUFFER, ID); }
    void unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
    void deleteShadowMap() { glDeleteFramebuffers(1, &ID); }

private:
    glm::vec2 size;

    float nearPlane = 1.0f;
    float farPlane = 100.0f;
    glm::mat4 lightProjection;
    glm::mat4 lightView;
    glm::mat4 lightVP;
};

#endif //OPENGL_GAMEENGINE_SHADOWMAP_HPP
