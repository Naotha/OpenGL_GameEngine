#ifndef OPENGL_GAMEENGINE_ENTITY_HPP
#define OPENGL_GAMEENGINE_ENTITY_HPP

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <memory>
#include "Engine/model.hpp"
#include "Engine/shader.hpp"

class Transform{
public:
    void CalculateModelMatrix()
    {
        _modelMatrix = GetLocalMatrix();
    }

    void CalculateModelMatrix(glm::mat4& parentModelMatrix)
    {
        _modelMatrix = parentModelMatrix * GetLocalMatrix();
    }

    glm::mat4& GetModelMatrix() { return _modelMatrix; }
    glm::vec3 GetLocalPosition() { return _position; }
    glm::quat GetLocalRotation() { return  _rotation; }
    glm::vec3 GetLocalScale() { return _scale; }

    void SetPosition(glm::vec3 newPosition)
    {
        _position = newPosition;
    }

    void SetRotation(glm::quat newRotation)
    {
        _rotation = newRotation;
    }

    void SetScale(glm::vec3 newScale)
    {
        _scale = newScale;
    }

    bool IsUpdated() { return _isUpdated; }

private:
    glm::mat4 GetLocalMatrix()
    {
         glm::mat4 translation = glm::translate(glm::mat4(1.0f), _position);

         glm::mat4 rotation = glm::mat4_cast(_rotation);;

         glm::mat4 scale = glm::scale(glm::mat4(1.0f), _scale);

         return translation * rotation * scale;
    }

    glm::vec3 _position = {0.0f, 0.0f, 0.0f};
    glm::quat _rotation = {0.0f, 0.0f, 0.0f, 0.0f};
    glm::vec3 _scale = {1.0f, 1.0f, 1.0f};

    glm::mat4 _modelMatrix = glm::mat4(1.0f);

    bool _isUpdated = true;
};

class Entity{
public:
    Entity(Model* model) { _model = model;}

    void AddChild(Entity* entity)
    {
        entity->SetParent(this);
        _children.push_back(entity);
    }

    void SetParent(Entity* parent)
    {
        _parent = parent;
    }

    void Update()
    {
        if (_parent != nullptr)
        {
            transform.CalculateModelMatrix(_parent->transform.GetModelMatrix());
        }
        else
        {
            transform.CalculateModelMatrix();
        }

        for (auto child : _children)
        {
            child->Update();
        }
    }

    void Render(Shader& shader)
    {
        _model->draw(shader, transform.GetModelMatrix());

        for (auto child : _children)
        {
            child->Render(shader);
        }
    }

    std::vector<Entity*> GetChildren()
    {
        return _children;
    }

    Transform transform;

private:
    std::vector<Entity*> _children;
    Entity* _parent = nullptr;
    Model* _model;
};

#endif //OPENGL_GAMEENGINE_ENTITY_HPP
