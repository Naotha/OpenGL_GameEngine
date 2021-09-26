#ifndef MESH_HPP
#define MESH_HPP

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <iostream>

#include "Engine/shader.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class Mesh
{
public:
    Mesh(float* vertices, unsigned int vertSize, unsigned int* indices, unsigned int indSize, Shader* shader);
    Mesh(float* vertices, unsigned int vertSize, unsigned int* indices, unsigned int indSize, Shader* shader, const char* texturePath, unsigned int texColorChannels);

    void draw();

    void addTextureUnit(const char* texturePath, unsigned int texColorChannels);
    
    void transform(glm::mat4 &transMatrix);

private:
    unsigned int _VBO;
    unsigned int _EBO;
    unsigned int _VAO;

    float* _vertices;
    unsigned int _verticesSize;
    unsigned int* _indices;
    unsigned int _indicesSize;
    Shader* _shader;

    glm::mat4 _transMatrix;
    
    unsigned int _textures[16];
    unsigned int _texUnitCount;

    void _createBufferObjects();
};

Mesh::Mesh(float* vertices, unsigned int vertSize, unsigned int* indices, unsigned int indSize, Shader* shader)
{
    _vertices = vertices;
    _verticesSize = vertSize;
    _indices = indices;
    _indicesSize = indSize;
    _shader = shader;

    _transMatrix = glm::mat4(1.0f);

    _texUnitCount = 0;
    
    _createBufferObjects();
}

Mesh::Mesh(float* vertices, unsigned int vertSize, unsigned int* indices, unsigned int indSize, Shader* shader, const char* texturePath, unsigned int texColorChannels = GL_RGB)
{
    _vertices = vertices;
    _verticesSize = vertSize;
    _indices = indices;
    _indicesSize = indSize;
    _shader = shader;

    _texUnitCount = 0;
    addTextureUnit(texturePath, texColorChannels);

    _createBufferObjects();
}

void Mesh::_createBufferObjects()
{
    // Create VBO, EBO, VAO
    glGenVertexArrays(1, &_VAO);
    glGenBuffers(1, &_VBO);
    glGenBuffers(1, &_EBO);
    // Bind VAO
    glBindVertexArray(_VAO);
    // Copy vertices array to VBO
    glBindBuffer(GL_ARRAY_BUFFER, _VBO);
    glBufferData(GL_ARRAY_BUFFER, _verticesSize, _vertices, GL_STATIC_DRAW);
    // Copy indices array to EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indicesSize, _indices, GL_STATIC_DRAW);
    // Set the vertex attributes pointers
    // Position
    glEnableVertexAttribArray(0); 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);  // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glVertexAttribPointer.xhtml
    // Color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    // Texture
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    glBindVertexArray(0);
}

void Mesh::addTextureUnit(const char* texturePath, unsigned int texColorChannels = GL_RGB)
{
    /* Initialize Texture */
    // Load image data
    int imWidth, imHeight, imChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(texturePath, &imWidth, &imHeight, &imChannels, 0);
    // Create texture
    glGenTextures(1, &_textures[_texUnitCount]);
    glBindTexture(GL_TEXTURE_2D, _textures[_texUnitCount]);
    _texUnitCount += 1,
    // Set texture wraping/filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Use image data as texture
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, texColorChannels, imWidth, imHeight, 0, texColorChannels, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture image data" << std::endl;
    }
    stbi_image_free(data);
}

void Mesh::transform(glm::mat4 &transMatrix)
{
    _transMatrix = transMatrix;
}

void Mesh::draw()
{
    // Set transform
    unsigned int transformLoc = glGetUniformLocation(_shader->getID(), "model");
    glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(_transMatrix));
    // Use Shader Program
    _shader->use();
    // Activate Texture Units
    for (unsigned int i = 0; i < _texUnitCount; i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, _textures[i]);
    }
    // Draw
    glBindVertexArray(_VAO);
    glDrawElements(GL_TRIANGLES, _indicesSize, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

}

#endif