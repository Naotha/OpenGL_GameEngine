#ifndef SHADER_HPP
#define SHADER_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>

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
    void use() const;
    // Set View and Projection Matrices
    void setViewMatrix(glm::mat4 &viewMatrix, const std::string &name);
    void setProjectionMatrix(glm::mat4 &projMatrix, const std::string &name);
    // Getters
    unsigned int getID() const;
private:
    // The ShaderProgram ID
    unsigned int _ID;
};

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
    // Retrive the vertex/fragment shader source codes from their path
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
        // Convert stream to stringg
        vertexCode = vShaderStream.str();
        fragmentCode = fShaderStream.str();

    }
    catch(const std::exception& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
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
    glShaderSource(vertexShader, 1, &vShaderCode, NULL);
    glCompileShader(vertexShader);
    // Check compilation
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLogVert);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLogVert << std::endl;
    }

    // Fragment Shader
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, NULL);
    glCompileShader(fragmentShader);
    // Check compilation
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLogFrag);
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
        glGetProgramInfoLog(_ID, 512, NULL, infoLogLink);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLogLink << std::endl;
    }
    // Delete linked Shader Objects
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::use() const
{
    glUseProgram(_ID);
}

void Shader::setViewMatrix(glm::mat4 &viewMatrix, const std::string &uniformValue)
{
    unsigned int viewLoc = glGetUniformLocation(_ID, uniformValue.c_str());
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(viewMatrix));
}

void Shader::setProjectionMatrix(glm::mat4 &projMatrix, const std::string &uniformValue)
{
    unsigned int projectionLoc = glGetUniformLocation(_ID, uniformValue.c_str());
    glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projMatrix));
}

unsigned int Shader::getID() const
{
    return _ID;
}

#endif