#pragma once

#include <glad/glad.h>
#include <vector>

#include "Engine/VBO.hpp"

class EBO
{
public:
    unsigned int ID;

    EBO(std::vector<unsigned int> indices)
    {
        glGenBuffers(1, &ID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    }

    void bind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID); }
    void unbind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
    void deleteEBO() { glDeleteBuffers(1, &ID); }
};