#ifndef MESH_HPP
#define MESH_HPP

#include <glad/glad.h>
#include <vector>

#include "Engine/shader.hpp"
#include "Engine/texture.hpp"
#include "Engine/VAO.hpp"
#include "Engine/VBO.hpp"
#include "Engine/EBO.hpp"

class Mesh
{
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices);
    Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures);

    void draw(Shader& shader);
private:
    VAO _VAO;

    void _createBufferObjects();
};

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices)
{
    this->vertices = vertices;
    this->indices = indices;
    textures = std::vector<Texture>();
    _createBufferObjects();
}

Mesh::Mesh(std::vector<Vertex>& vertices, std::vector<unsigned int>& indices, std::vector<Texture>& textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    _createBufferObjects();
}

void Mesh::_createBufferObjects()
{
    // Create VBO, EBO
    _VAO.bind();
    VBO _VBO = VBO(vertices);
    EBO _EBO = EBO(indices);

    // Set the vertex attribute pointers
    // Position
    _VAO.linkAttribPointer(_VBO, 0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // Normal
    _VAO.linkAttribPointer(_VBO, 1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    // Texture
    _VAO.linkAttribPointer(_VBO, 2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));

    // Unbin buffers
    _VAO.unbind();
    _VBO.unbind();
    _EBO.unbind();
}

void Mesh::draw(Shader& shader)
{
    // Use Shader Program
    shader.bind();
    // Activate Texture Units
    unsigned int diffuseCount = 1;
    unsigned int specularCount = 1;
    for (unsigned int i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        std::string name;
        std::string number;
        TextureType actualType = textures[i].getType();
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
        glBindTexture(GL_TEXTURE_2D, textures[i].getID());
    }
    // Draw
    _VAO.bind();
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glActiveTexture(GL_TEXTURE0);
    shader.unbind();
}

#endif