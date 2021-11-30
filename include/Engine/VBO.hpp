#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 texCoord;
};

class VBO
{
public:
    unsigned int ID;

    VBO(std::vector<Vertex> vertices)
    {
        glGenBuffers(1, &ID);
        glBindBuffer(GL_ARRAY_BUFFER, ID);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    }

    void bind() { glBindBuffer(GL_ARRAY_BUFFER, ID); }
    void unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }
    void deleteVBO() { glDeleteBuffers(1, &ID); }
};