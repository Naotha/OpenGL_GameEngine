#ifndef OPENGL_GAMEENGINE_SCENE_HPP
#define OPENGL_GAMEENGINE_SCENE_HPP

#include "GameObject.hpp"

class Scene{
public:
    Scene() : _gameObjects() {};

    void AddGameObject(GameObject* gameObject)
    {
        _gameObjects.push_back(gameObject);
    }

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

    GameObject* CreateGameObject()
    {
        _gameObjects.push_back(new GameObject());
        return _gameObjects.back();
    }

private:
    std::vector<GameObject*> _gameObjects;
};

#endif //OPENGL_GAMEENGINE_SCENE_HPP
