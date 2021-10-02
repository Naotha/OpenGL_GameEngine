#ifndef CAMERA_HPP
#define CMAERA_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const float DEFAULT_YAW = -90.0f;
const float DEFAULT_PITCH = 0.0f;
const float DEFAULT_SPEED = 2.5f;
const float DEFAULT_SENSITIVITY = 0.1f;
const float DEFAULT_FOV = 45.0f;

enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
};

class Camera
{
public:
    Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float movementSpeed, float mouseSensitivity, float fov);

    void processKeyboard(Camera_Movement direction, float deltaTime);
    void processMouseMovement(float xOffset, float yOffset);
    void processMouseScroll(float yOffset);

    glm::mat4 getViewMatrix();
    float getFOV();

private:
    glm::vec3 _position;
    glm::vec3 _front;
    glm::vec3 _worldUp;
    glm::vec3 _up;
    glm::vec3 _right;

    float _yaw;
    float _pitch;

    float _movementSpeed;
    float _mouseSensitivity;
    float _fov;

    void _updateCameraVectors();
};

Camera::Camera(glm::vec3 position, glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = DEFAULT_YAW, float pitch = DEFAULT_PITCH, float movementSpeed = DEFAULT_SPEED, float mouseSensitivity = DEFAULT_SENSITIVITY, float fov = DEFAULT_FOV)
{
    _position = position;
    _front = glm::vec3(0.0f, 0.0f, -1.0f);
    _worldUp = worldUp;

    _yaw = yaw;
    _pitch = pitch;

    _movementSpeed = movementSpeed;
    _mouseSensitivity = mouseSensitivity;
    _fov = fov;

    _updateCameraVectors();
}

void Camera::_updateCameraVectors()
{
    glm::vec3 direction;
    direction.x = cos(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    direction.y = sin(glm::radians(_pitch));
    direction.z = sin(glm::radians(_yaw)) * cos(glm::radians(_pitch));
    _front = glm::normalize(direction);

    _right = glm::normalize(glm::cross(_front, _worldUp));
    _up = glm::normalize(glm::cross(_right, _front));
}

void Camera::processKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = _movementSpeed * deltaTime;
    if (direction == FORWARD)
        _position += velocity * _front;
    if (direction == BACKWARD)
        _position -= velocity * _front;
    if (direction == LEFT)
        _position -= velocity * _right;
    if (direction == RIGHT)
        _position += velocity * _right;
}

void Camera::processMouseMovement(float xOffset, float yOffset)
{
    xOffset *= _mouseSensitivity;
    yOffset *= _mouseSensitivity;

    _yaw += xOffset;
    _pitch += yOffset;
    if (_pitch > 89.0f)
        _pitch = 89.0f;
    if (_pitch < -89.0f)
        _pitch = -89.0f;

    _updateCameraVectors();
}

void Camera::processMouseScroll(float yOffset)
{
    _fov -= (float)yOffset;
    if (_fov < 1.0f)
        _fov = 1.0f;
    if (_fov > 45.0f)
        _fov = 45.0f;
}

glm::mat4 Camera::getViewMatrix()
{
    return glm::lookAt(_position, _position + _front, _up);
}

float Camera::getFOV()
{
    return _fov;
}

#endif