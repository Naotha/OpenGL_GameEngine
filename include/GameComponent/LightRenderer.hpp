#ifndef OPENGL_GAMEENGINE_LIGHTRENDERER_HPP
#define OPENGL_GAMEENGINE_LIGHTRENDERER_HPP

#include "GameComponent.hpp"
#include "Engine/light.hpp"
#include "Renderer/Renderer.h"

class DirectionalLightRenderer : public GameComponent{
public:
    DirectionalLightRenderer(DirectionalLight& light, Shader& shader) : light(light), shader(shader)
    {
        enabled = true;
        Renderer::GetInstance()->AddShader(&this->shader);
    };

    void Render(Transform transform)
    {
        if (enabled)
        {
            light.setLightInShader("u_dirLight", shader);
        }
    }

    DirectionalLight& GetLight()
    {
        return light;
    }

private:
    DirectionalLight light;
    Shader shader;
};

class PointLightRenderer : public GameComponent{
public:
    PointLightRenderer(PointLight& light, Shader& shader) : light(light), shader(shader)
    {
        shaderIndex = Renderer::GetInstance()->GetPointLightCount();
        Renderer::GetInstance()->AddPointLight();

        enabled = true;
        Renderer::GetInstance()->AddShader(&this->shader);
    };

    void Render(Transform transform)
    {
        if (enabled)
        {
            light.setLightInShader("u_pointLights["+ std::to_string(shaderIndex) +"]", shader);
        }
    }

    PointLight& GetPointLight()
    {
        return light;
    }

private:
    PointLight light;
    Shader shader;

    int shaderIndex;
};

class SpotLightRenderer : public GameComponent{
public:
    SpotLightRenderer(SpotLight& light, Shader& shader) : light(light), shader(shader)
    {
        shaderIndex = Renderer::GetInstance()->GetSpotLightCount();
        Renderer::GetInstance()->AddSpotLight();

        enabled = true;
        Renderer::GetInstance()->AddShader(&this->shader);
    };

    void Render(Transform transform)
    {
        light.setLightInShader("u_spotLights["+ std::to_string(shaderIndex) +"]", shader);
    }

    SpotLight& GetSpotLight()
    {
        return light;
    }

    void Disable() override
    {
        enabled = false;
        ambient = light.getAmbient();
        diffuse = light.getDiffuse();
        specular = light.getSpecular();
        light.setAmbient(glm::vec3(0.0f));
        light.setDiffuse(glm::vec3(0.0f));
        light.setSpecular(glm::vec3(0.0f));
    }

    void Enable() override
    {
        light.setAmbient(ambient);
        light.setDiffuse(diffuse);
        light.setSpecular(specular);
    }

private:
    SpotLight light;
    Shader shader;

    int shaderIndex;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

#endif //OPENGL_GAMEENGINE_LIGHTRENDERER_HPP
