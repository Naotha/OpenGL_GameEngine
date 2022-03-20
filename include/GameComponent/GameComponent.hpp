#ifndef OPENGL_GAMEENGINE_GAMECOMPONENT_HPP
#define OPENGL_GAMEENGINE_GAMECOMPONENT_HPP

#include "Engine/Transform.hpp"

class GameComponent{
public:
    virtual void Update(Transform transform) {};
    virtual void Render(Transform transform) {};
};

#endif //OPENGL_GAMEENGINE_GAMECOMPONENT_HPP
