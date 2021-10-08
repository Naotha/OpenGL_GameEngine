#pragma once

#include <glad/glad.h>
#include <vector>

#include "Engine/shader.hpp"
#include "Engine/texture.hpp"
#include "Engine/vertex.h"

class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;

    Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, Shader* shader);
    Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures, Shader* shader);

    void setTextures(std::vector<Texture>& textures);

    void draw();
private:
    unsigned int _VBO;
    unsigned int _EBO;
    unsigned int _VAO;

    std::vector<Texture> _textures;
    Shader* _shader;

    void _createBufferObjects();
};

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, Shader* shader)
{
    this->vertices = vertices;
    this->indices = indices;
    _shader = shader;

    _textures = std::vector<Texture>();
    
    _createBufferObjects();
}

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures, Shader* shader)
{
    this->vertices = vertices;
    this->indices = indices;
    _textures = textures;
    _shader = shader;

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
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    // Copy indices array to EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    // Set the vertex attributes pointers
    // Position
    glEnableVertexAttribArray(0); 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);  // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glVertexAttribPointer.xhtml
    // Color
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // Texture
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

    glBindVertexArray(0);
}

void Mesh::setTextures(std::vector<Texture>& textures)
{
    _textures = textures;
}

void Mesh::draw()
{
    // Use Shader Program
    _shader->use();
    // Activate Texture Units
    for (unsigned int i = 0; i < _textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        _shader->setUniformInt(_textures[i].getUniformName(), i);
        glBindTexture(GL_TEXTURE_2D, _textures[i].getID());
    }
    // Draw
    glBindVertexArray(_VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
    _shader->unbind();
}