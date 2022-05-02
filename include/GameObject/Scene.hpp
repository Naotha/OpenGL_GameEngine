#ifndef OPENGL_GAMEENGINE_SCENE_HPP
#define OPENGL_GAMEENGINE_SCENE_HPP

#include <colony/plf_colony.h>
#include "GameObject.hpp"

class Scene{
public:
    Scene() : _gameObjects() {};

    void Update()
    {
        for (auto gameObject : _gameObjects)
        {
            gameObject->Update();
        }
    }

    void Render()
    {
        for (auto gameObject : _gameObjects)
        {
            gameObject->Render();
        }
    }

    void RenderWithShader(Shader& shader)
    {
        for (auto gameObject : _gameObjects)
        {
            gameObject->RenderWithShader(shader);
        }
    }

    void RenderLightsOnly(Shader& shader)
    {
        for (auto gameObject : _gameObjects)
        {
            gameObject->RenderLightsOnly(shader);
        }
    }

    GameObject* CreateGameObject()
    {
        auto gameObject = new GameObject();
        _gameObjects.insert(gameObject);
        return gameObject;
    }

private:
    plf::colony<GameObject*> _gameObjects;
};

#endif //OPENGL_GAMEENGINE_SCENE_HPP
