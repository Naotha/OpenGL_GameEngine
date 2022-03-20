#ifndef OPENGL_GAMEENGINE_MODELRENDERER_HPP
#define OPENGL_GAMEENGINE_MODELRENDERER_HPP

#include "GameComponent.hpp"
#include "Engine/model.hpp"
#include "Engine/shader.hpp"

class ModelRenderer : public GameComponent{
public:
    ModelRenderer(Model* model, Shader& shader) : model(model), shader(shader) {};

    void Render(Transform transform) override
    {
        model->draw(shader, transform.GetModelMatrix());
    }

private:
    Model* model;
    Shader& shader;
};

#endif //OPENGL_GAMEENGINE_MODELRENDERER_HPP
