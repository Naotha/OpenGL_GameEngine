#pragma once

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
    DirectionalLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) : _direction(direction)
    {
        _ambient = ambient;
        _diffuse = diffuse;
        _specular = specular;
    };

    glm::vec3 getDirection() { return _direction; }

    void setDirection(glm::vec3 direction) { _direction = direction; }

    void setLightInShader(std::string uniformLightName, Shader& shader) final override
    {
        shader.use();
        shader.setUniformFloat3((uniformLightName + ".direction").c_str(), _direction);

        shader.setUniformFloat3((uniformLightName + ".ambient").c_str(), _ambient);
        shader.setUniformFloat3((uniformLightName + ".diffuse").c_str(), _diffuse);
        shader.setUniformFloat3((uniformLightName + ".specular").c_str(), _specular);
        shader.unbind();
    }
private:
    glm::vec3 _direction;
};

class PointLight: public Light
{
public:
    PointLight(glm::vec3 position, float constAttenuation, float linAttenuation, float quadAttenuation,
               glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) :
               _position(position), _constAttenuation(constAttenuation), _linAttenuation(linAttenuation), _quadAttenuation(quadAttenuation) 
    {
        _ambient = ambient;
        _diffuse = diffuse;
        _specular = specular;
    };

    glm::vec3 getPosition() { return _position; }
    float getConstAttenuation() { return _constAttenuation; }
    float getLinAttenuation() { return _linAttenuation; }
    float getQuadAttenuation() { return _quadAttenuation; }

    void setPosition(glm::vec3 position) { _position = position; }
    void setConstAttenuation(float constAttenuation) { _constAttenuation = constAttenuation; }
    void setLinAttenuation(float linAttenuation) { _linAttenuation = linAttenuation; }
    void setQuadAttenuation(float quadAttenuation) { _quadAttenuation = quadAttenuation; }

    void setLightInShader(std::string uniformLightName, Shader& shader) final override
    {
        shader.use();
        shader.setUniformFloat3(uniformLightName.append(".position").c_str(), _position);

        shader.setUniformFloat((uniformLightName + ".constant").c_str(), _constAttenuation);
        shader.setUniformFloat((uniformLightName + ".linear").c_str(), _linAttenuation);
        shader.setUniformFloat((uniformLightName + ".quadratic").c_str(), _quadAttenuation);

        shader.setUniformFloat3((uniformLightName + ".ambient").c_str(), _ambient);
        shader.setUniformFloat3((uniformLightName + ".diffuse").c_str(), _diffuse);
        shader.setUniformFloat3((uniformLightName + ".specular").c_str(), _specular);
        shader.unbind();
    }
private:
    glm::vec3 _position;

    float _constAttenuation;
    float _linAttenuation;
    float _quadAttenuation;
};

class SpotLight: public Light
{
public:
    SpotLight(glm::vec3 position, glm::vec3 direction, float innerCutOff, float outerCutOff,
              glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular) :
    _position(position), _direction(direction), _innerCutOff(innerCutOff), _outerCutOff(outerCutOff)
    {
        _ambient = ambient;
        _diffuse = diffuse;
        _specular = specular;
    };

    glm::vec3 getPosition() { return _position; }
    glm::vec3 getDirection() { return _direction; }
    float getInnerCutOff() { return _innerCutOff; }
    float getOuterCutOff() { return _outerCutOff; }

    void setPosition(glm::vec3 position) { _position = position; }
    void setDirection(glm::vec3 direction) { _direction = direction; }
    void setInnerCutOff(float innerCutOff) { _innerCutOff = innerCutOff; }
    void setOuterCutOff(float outerCutOff) { _outerCutOff = outerCutOff; }

    void setLightInShader(std::string uniformLightName, Shader& shader) final override
    {
        shader.use();
        shader.setUniformFloat3((uniformLightName + ".position").c_str(), _position);
        shader.setUniformFloat3((uniformLightName + ".position").c_str(), _direction);

        shader.setUniformFloat((uniformLightName + ".innerCutOff").c_str(), _innerCutOff);
        shader.setUniformFloat((uniformLightName + ".outerCutOff").c_str(), _outerCutOff);

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
};