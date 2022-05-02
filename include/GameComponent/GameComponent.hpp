#ifndef OPENGL_GAMEENGINE_GAMECOMPONENT_HPP
#define OPENGL_GAMEENGINE_GAMECOMPONENT_HPP

#include "Engine/Transform.hpp"

class GameComponent{
public:
    virtual void Update(Transform transform) {};
    virtual void Render(Transform transform) {};
    virtual void RenderWithShader(Transform transform, Shader shader) {};
    virtual void RenderLightsOnly(Transform transform, Shader shader) {};

    virtual void Enable() { enabled = true; };
    virtual void Disable() { enabled = false; };
    virtual bool IsEnabled() {return true;};

protected:
    bool enabled;
};

#endif //OPENGL_GAMEENGINE_GAMECOMPONENT_HPP
