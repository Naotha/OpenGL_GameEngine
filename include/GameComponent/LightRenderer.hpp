#ifndef OPENGL_GAMEENGINE_LIGHTRENDERER_HPP
#define OPENGL_GAMEENGINE_LIGHTRENDERER_HPP

#include "GameComponent.hpp"
#include "Engine/light.hpp"

class LightRenderer : public GameComponent{
public:
    LightRenderer(Light* light, LIGHT_TYPE type, Shader& shader) : light(light), type(type), shader(shader) {};

    void Render(Transform transform)
    {
        if (type == LIGHT_TYPE::POINT_LIGHT)
        {
            light->setLightInShader("u_pointLights[0]", shader);
            shader.bind();
            shader.setUniformInt("u_pointLightsNum", 1);
            shader.unbind();
        }
        else if (type == LIGHT_TYPE::SPOT_LIGHT)
        {
            light->setLightInShader("u_spotLights[0]", shader);
            shader.bind();
            shader.setUniformInt("u_spotLightsNum", 1);
            shader.unbind();
        }
        else if (type == LIGHT_TYPE::DIRECTIONAL_LIGHT)
        {
            light->setLightInShader("u_dirLight", shader);
        }
    }

private:
    Light* light;
    LIGHT_TYPE type;
    Shader& shader;
};

#endif //OPENGL_GAMEENGINE_LIGHTRENDERER_HPP
