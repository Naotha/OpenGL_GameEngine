#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <glm/glm.hpp>
#include "Engine/shader.hpp"
#include <string>
#include <iostream>

class Light
{
public:
    void setAmbient(glm::vec3 ambient) { _ambient = ambient; }
    void setDiffuse(glm::vec3 diffuse) { _diffuse = diffuse; }
    void setSpecular(glm::vec3 specular) { _specular = specular; }

    virtual void setLightInShader(std::string uniformLightName, Shader& shader) = 0;
protected:
    glm::vec3 _ambient;
    glm::vec3 _diffuse;
    glm::vec3 _specular;
};

class DirectionalLight: public Light
{
public:
    DirectionalLight() = default;

    DirectionalLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) : _direction(direction)
    {
        _ambient = ambient;
        _diffuse = diffuse;
        _specular = specular;
    };

    glm::vec3 getDirection() { return _direction; }

    void setDirection(glm::vec3 direction) { _direction = direction; }

    void setLightInShader(std::string uniformLightName, Shader& shader) final
    {
        shader.bind();
        shader.setUniformFloat3((uniformLightName + ".direction").c_str(), _direction);

        shader.setUniformFloat3((uniformLightName + ".ambient").c_str(), _ambient);
        shader.setUniformFloat3((uniformLightName + ".diffuse").c_str(), _diffuse);
        shader.setUniformFloat3((uniformLightName + ".specular").c_str(), _specular);
        shader.unbind();
    }
private:
    glm::vec3 _direction;
};

struct Attenuation
{
    float constant;
    float linear;
    float quadratic;
};

const Attenuation CONST_ATTENUATION{1.0f, 0.09f, 0.032f};

class PointLight: public Light
{
public:
    PointLight() = default;

    PointLight(glm::vec3 position, Attenuation attenuation,
               glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) :
               _position(position), _attenuation(attenuation)
    {
        _ambient = ambient;
        _diffuse = diffuse;
        _specular = specular;
    };

    glm::vec3 getPosition() { return _position; }
    float getConstAttenuation() { return _attenuation.constant; }
    float getLinAttenuation() { return _attenuation.linear; }
    float getQuadAttenuation() { return _attenuation.quadratic; }

    void setPosition(glm::vec3 position) { _position = position; }
    void setConstAttenuation(float constAttenuation) { _attenuation.constant = constAttenuation; }
    void setLinAttenuation(float linAttenuation) { _attenuation.linear = linAttenuation; }
    void setQuadAttenuation(float quadAttenuation) { _attenuation.quadratic = quadAttenuation; }

    void setLightInShader(std::string uniformLightName, Shader& shader) final
    {
        shader.bind();
        shader.setUniformFloat3((uniformLightName + ".position").c_str(), _position);

        shader.setUniformFloat((uniformLightName + ".constant").c_str(), _attenuation.constant);
        shader.setUniformFloat((uniformLightName + ".linear").c_str(), _attenuation.linear);
        shader.setUniformFloat((uniformLightName + ".quadratic").c_str(), _attenuation.quadratic);

        shader.setUniformFloat3((uniformLightName + ".ambient").c_str(), _ambient);
        shader.setUniformFloat3((uniformLightName + ".diffuse").c_str(), _diffuse);
        shader.setUniformFloat3((uniformLightName + ".specular").c_str(), _specular);
        shader.unbind();
    }
private:
    glm::vec3 _position;

    Attenuation _attenuation;
};

class SpotLight: public Light
{
public:
    SpotLight() = default;

    SpotLight(glm::vec3 position, glm::vec3 direction, float innerCutOff, float outerCutOff,
              Attenuation attenuation, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) :
    _position(position), _direction(direction), _innerCutOff(innerCutOff), _outerCutOff(outerCutOff),
    _attenuation(attenuation)
    {
        _ambient = ambient;
        _diffuse = diffuse;
        _specular = specular;
    };

    glm::vec3 getPosition() { return _position; }
    glm::vec3 getDirection() { return _direction; }
    float getInnerCutOff() { return _innerCutOff; }
    float getOuterCutOff() { return _outerCutOff; }
    float getConstAttenuation() { return _attenuation.constant; }
    float getLinAttenuation() { return _attenuation.linear; }
    float getQuadAttenuation() { return _attenuation.quadratic; }

    void setPosition(glm::vec3 position) { _position = position; }
    void setDirection(glm::vec3 direction) { _direction = direction; }
    void setInnerCutOff(float innerCutOff) { _innerCutOff = innerCutOff; }
    void setOuterCutOff(float outerCutOff) { _outerCutOff = outerCutOff; }
    void setConstAttenuation(float constAttenuation) { _attenuation.constant = constAttenuation; }
    void setLinAttenuation(float linAttenuation) { _attenuation.linear = linAttenuation; }
    void setQuadAttenuation(float quadAttenuation) { _attenuation.quadratic = quadAttenuation; }

    void setLightInShader(std::string uniformLightName, Shader& shader) final
    {
        shader.bind();
        shader.setUniformFloat3((uniformLightName + ".position").c_str(), _position);
        shader.setUniformFloat3((uniformLightName + ".direction").c_str(), _direction);

        shader.setUniformFloat((uniformLightName + ".innerCutOff").c_str(), _innerCutOff);
        shader.setUniformFloat((uniformLightName + ".outerCutOff").c_str(), _outerCutOff);

        shader.setUniformFloat((uniformLightName + ".constant").c_str(), _attenuation.constant);
        shader.setUniformFloat((uniformLightName + ".linear").c_str(), _attenuation.linear);
        shader.setUniformFloat((uniformLightName + ".quadratic").c_str(), _attenuation.quadratic);

        shader.setUniformFloat3((uniformLightName + ".ambient").c_str(), _ambient);
        shader.setUniformFloat3((uniformLightName + ".diffuse").c_str(), _diffuse);
        shader.setUniformFloat3((uniformLightName + ".specular").c_str(), _specular);
        shader.unbind();
    }
private:
    glm::vec3 _position;
    glm::vec3 _direction;
    
    float _innerCutOff;
    float _outerCutOff;

    Attenuation _attenuation;
};

#endif