#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
    // Constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);
    // Activates the shader
    void bind() const;
    void unbind() const;
    // Setters for Uniform Values
    void setUniformInt(const char* name, int value);
    void setUniformIntArray(const char* name, unsigned long count, int* values);
    void setUniformFloat(const char* name, float value);
    void setUniformFloat2(const char* name, const glm::vec2& value);
    void setUniformFloat3(const char* name, const glm::vec3& value);
    void setUniformFloat4(const char* name, const glm::vec4& value);
    void setUniformMat3(const char* name, const glm::mat3& value);
    void setUniformMat4(const char* name, const glm::mat4& value);
    // Getters
    unsigned int getID() const;
private:
    // The ShaderProgram ID
    unsigned int _ID;
};

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    // Retrieve the vertex/fragment shader source codes from their path
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // Ensure ifstream objects can throw exceptions
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        // Open files
        vShaderFile.open(vertexPath);
        fShaderFile.open(fragmentPath);
        // Read files' content
        std::stringstream vShaderStream;
        std::stringstream fShaderStream;
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // Close files
        vShaderFile.close();
        fShaderFile.close();
        // Convert stream to string
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

    }
    catch(const std::exception& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
    }    
    
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    // Compile shaders
    unsigned int vertexShader;
    unsigned int fragmentShader;
    int success;
    char infoLogVert[512];
    char infoLogFrag[512];
    char infoLogLink[512];

    // Vertex Shader
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, nullptr);
    glCompileShader(vertexShader);
    // Check compilation
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLogVert);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLogVert << std::endl;
    }

    // Fragment Shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, nullptr);
    glCompileShader(fragmentShader);
    // Check compilation
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infoLogFrag);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLogFrag << std::endl;
    }

    // Shader program
    _ID = glCreateProgram();
    glAttachShader(_ID, vertexShader);
    glAttachShader(_ID, fragmentShader);
    glLinkProgram(_ID);
    // Check linking
    if (!success)
    {
        glGetProgramInfoLog(_ID, 512, nullptr, infoLogLink);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLogLink << std::endl;
    }
    // Delete linked Shader Objects
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::bind() const
{
    glUseProgram(_ID);
}

void Shader::unbind() const
{
    glUseProgram(0);
}


unsigned int Shader::getID() const
{
    return _ID;
}

void Shader::setUniformInt(const char* name, int value)
{
    glUniform1i(glGetUniformLocation(_ID, name), value);
}
void Shader::setUniformIntArray(const char* name, unsigned long count, int* values)
{
    glUniform1iv(glGetUniformLocation(_ID, name), count, values);
}
void Shader::setUniformFloat(const char* name, float value)
{
    glUniform1f(glGetUniformLocation(_ID, name), value);
}
void Shader::setUniformFloat2(const char* name, const glm::vec2& value)
{
    glUniform2f(glGetUniformLocation(_ID, name), value.x, value.y);
}
void Shader::setUniformFloat3(const char* name, const glm::vec3& value)
{
    glUniform3f(glGetUniformLocation(_ID, name), value.x, value.y, value.z);
}
void Shader::setUniformFloat4(const char* name, const glm::vec4& value)
{
    glUniform4f(glGetUniformLocation(_ID, name), value.x, value.y, value.z, value.w);
}
void Shader::setUniformMat3(const char* name, const glm::mat3& value)
{
    glUniformMatrix3fv(glGetUniformLocation(_ID, name), 1, GL_FALSE, glm::value_ptr(value));
}
void Shader::setUniformMat4(const char* name, const glm::mat4& value)
{
    glUniformMatrix4fv(glGetUniformLocation(_ID, name), 1, GL_FALSE, glm::value_ptr(value));
}

#endif