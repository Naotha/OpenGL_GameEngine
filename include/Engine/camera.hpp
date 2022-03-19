#ifndef CAMERA_HPP
#define CAMERA_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

const float default_yaw = -90.0f;
const float default_pitch = 0.0f;
const float default_speed = 2.5f;
const float default_sensitivity = 0.1f;
const float default_fov = 45.0f;

enum CameraMovement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
};

class Camera
{
public:
    glm::vec3 position;
    float movementSpeed;
    float mouseSensitivity;
    float fov;

    Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch, float movementSpeed, float mouseSensitivity, float fov);

    void processKeyboard(CameraMovement direction, float deltaTime);
    void processMouseMovement(float xOffset, float yOffset);
    void processMouseScroll(float yOffset);

    glm::mat4 getViewMatrix() { return glm::lookAt(position, position + _front, _up); }
    glm::vec3 getFront() { return _front; }
private:
    glm::vec3 _worldUp;
    glm::vec3 _front;
    glm::vec3 _up;
    glm::vec3 _right;

    float _yaw;
    float _pitch;

    bool _isEnabled;

    void _updateCameraVectors();
};

Camera::Camera(glm::vec3 position, glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = default_yaw, float pitch = default_pitch, float movementSpeed = default_speed, float mouseSensitivity = default_sensitivity, float fov = default_fov)
{
    this->position = position;
    _front = glm::vec3(0.0f, 0.0f, -1.0f);
    _worldUp = worldUp;

    _yaw = yaw;
    _pitch = pitch;

    this->movementSpeed = movementSpeed;
    this->mouseSensitivity = mouseSensitivity;
    this->fov = fov;

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

void Camera::processKeyboard(CameraMovement direction, float deltaTime)
{
    float velocity = movementSpeed * deltaTime;
    if (direction == FORWARD)
        position += velocity * _front;
    if (direction == BACKWARD)
        position -= velocity * _front;
    if (direction == LEFT)
        position -= velocity * _right;
    if (direction == RIGHT)
        position += velocity * _right;
}

void Camera::processMouseMovement(float xOffset, float yOffset)
{
    xOffset *= mouseSensitivity;
    yOffset *= mouseSensitivity;

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
    fov -= (float)yOffset;
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

#endif