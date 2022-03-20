#ifndef OPENGL_GAMEENGINE_TRANSFORM_HPP
#define OPENGL_GAMEENGINE_TRANSFORM_HPP

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Transform{
public:
    void CalculateModelMatrix()
    {
        _modelMatrix = GetLocalMatrix();
        _isUpdated = false;
    }

    void CalculateModelMatrix(glm::mat4& parentModelMatrix)
    {
        _modelMatrix = parentModelMatrix * GetLocalMatrix();
        _isUpdated = false;
    }

    glm::mat4& GetModelMatrix() { return _modelMatrix; }
    glm::vec3 GetLocalPosition() { return _position; }
    glm::quat GetLocalRotation() { return  _rotation; }
    glm::vec3 GetLocalScale() { return _scale; }

    void SetPosition(glm::vec3 newPosition)
    {
        _isUpdated = true;
        _position = newPosition;
    }

    void SetRotation(glm::quat newRotation)
    {
        _isUpdated = true;
        _rotation = newRotation;
    }

    void SetScale(glm::vec3 newScale)
    {
        _isUpdated = true;
        _scale = newScale;
    }

    bool IsUpdated()
    {
        return _isUpdated;
    }

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

#endif //OPENGL_GAMEENGINE_TRANSFORM_HPP
