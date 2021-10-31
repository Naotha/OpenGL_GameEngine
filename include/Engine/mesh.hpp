#pragma once

#include <glad/glad.h>
#include <vector>

#include "Engine/shader.hpp"
#include "Engine/texture.hpp"

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> _textures;

    Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
    Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures);

    void draw(Shader& shader);
private:
    unsigned int _VBO;
    unsigned int _EBO;
    unsigned int _VAO;

    void _createBufferObjects();
};

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
{
    this->vertices = vertices;
    this->indices = indices;
    _textures = std::vector<Texture>();
    _createBufferObjects();
}

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures)
{
    this->vertices = vertices;
    this->indices = indices;
    _textures = textures;
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
    // Normal
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // Texture
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

    glBindVertexArray(0);
}

void Mesh::draw(Shader& shader)
{
    // Use Shader Program
    shader.use();
    // Activate Texture Units
    unsigned int diffuseCount = 1;
    unsigned int specularCount = 1;
    for (unsigned int i = 0; i < _textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        std::string name;
        std::string number;
        TextureType actualType = _textures[i].getType();
        if (actualType == TextureType::DIFFUSE)
        {
            name = "texture_diffuse";
            number = std::to_string(diffuseCount++);
        }
        else if (actualType == TextureType::SPECULAR)
        {
            name = "texture_specular";
            number = std::to_string(specularCount++);
        }
        shader.setUniformInt(("u_material." + name + number).c_str(), i);
        glBindTexture(GL_TEXTURE_2D, _textures[i].getID());
    }
    // Draw
    glBindVertexArray(_VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
    shader.unbind();
}