#pragma once

#include <glad/glad.h>

#include "Engine/VBO.hpp"

class VAO
{
public:
    unsigned int ID;

    VAO()
    {
        glGenVertexArrays(1, &ID);
    }

    void linkAttribPointer(VBO &VBO, GLuint layout, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *offset)
    {
        VBO.bind();
        glEnableVertexAttribArray(layout); 
        glVertexAttribPointer(layout, size, type, normalized, stride, offset);  // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glVertexAttribPointer.xhtml
        VBO.unbind();
    }

    void bind() { glBindVertexArray(ID); }
    void unbind() { glBindVertexArray(0); }
    void deleteVAO() { glDeleteVertexArrays(1, &ID); }

};