#ifndef OPENGL_GAMEENGINE_MODELRENDERER_HPP
#define OPENGL_GAMEENGINE_MODELRENDERER_HPP

#include "GameComponent.hpp"
#include "Engine/model.hpp"
#include "Engine/shader.hpp"

class ModelRenderer : public GameComponent{
public:
    ModelRenderer(Model* model, Shader& shader) : model(model), shader(shader)
    {
        enabled = true;
        Renderer::GetInstance()->AddShader(&this->shader);
    };

    void Render(Transform transform) override
    {
        if (enabled)
        {
            model->draw(shader, transform.GetModelMatrix());
            shader.bind();
            shader.setUniformFloat("u_material.shininess", 32.0f);
            shader.unbind();
        }
    }

    void RenderWithShader(Transform transform, Shader& shader) override
    {
        if (enabled)
        {
            model->draw(shader, transform.GetModelMatrix());
            shader.bind();
            shader.setUniformFloat("u_material.shininess", 32.0f);
            shader.unbind();
        }
    }

private:
    Model* model;
    Shader& shader;
};

#endif //OPENGL_GAMEENGINE_MODELRENDERER_HPP
