#ifndef OPENGL_GAMEENGINE_GAMEOBJECT_HPP
#define OPENGL_GAMEENGINE_GAMEOBJECT_HPP

#include <vector>
#include <memory>
#include "Engine/model.hpp"
#include "Engine/shader.hpp"
#include "Engine/Transform.hpp"
#include "GameComponent/GameComponent.hpp"

class GameObject{
public:
    GameObject() = default;

    void AddChild(GameObject* entity)
    {
        if (entity != nullptr)
        {
            entity->SetParent(this);
            _children.push_back(entity);
        }
    }

    void AddComponent(GameComponent* component)
    {
        if (component != nullptr)
            _components.push_back(component);
    }

    void SetParent(GameObject* parent)
    {
        _parent = parent;
    }

    void Update()
    {
        for (auto component : _components)
        {
            component->Update(transform);
        }

        for (auto child : _children)
        {
            child->Update();
        }
    }

    void Render()
    {
        for (auto component : _components)
        {
            component->Render(transform);
        }

        for (auto child : _children)
        {
            child->Render();
        }
    }

    void SetPosition(glm::vec3 newPosition)
    {
        transform.SetPosition(newPosition);
        UpdateTransform();
    }

    void SetRotation(glm::quat newRotation)
    {
        transform.SetRotation(newRotation);
        UpdateTransform();
    }

    void SetScale(glm::vec3 newScale)
    {
        transform.SetScale(newScale);
        UpdateTransform();
    }

    std::vector<GameObject*> GetChildren()
    {
        return _children;
    }

    Transform transform;

protected:
    void UpdateTransform()
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
            child->UpdateTransform();
        }
    }

    // Unique pointerek
    // "Fa" szerkezet helyett lehetne lista, ahol minden entity benne van alapbol, es a szulo az "indexel" hivatkozik ra
    //          - hatuluto: a vector nem jo adatstruktura erre -> 3rd party colony adatstruktura
    std::vector<GameObject*> _children;
    std::vector<GameComponent*> _components;
    GameObject* _parent = nullptr;
};

#endif //OPENGL_GAMEENGINE_GAMEOBJECT_HPP
