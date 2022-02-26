#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image/stb_image.h"
#include <iostream>
#include <glad/glad.h>

// TODO TextureType??? Enum for diffuse, specular etc
enum TextureType
{
    DIFFUSE,
    SPECULAR
};

class Texture
{
public:
    Texture(const char* path, TextureType type);

    unsigned int getID() { return _ID; }
    TextureType getType() { return _type; }
    const char* getPath() { return _path; }
private:
    unsigned int _ID;
    TextureType _type;
    const char* _path;
};

Texture::Texture(const char* path, TextureType type)
{
    _path = path;
    _type = type;
    /* Initialize Texture */
    // Load image data
    int imWidth, imHeight, imChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(_path, &imWidth, &imHeight, &imChannels, 0);
    // Create texture
    glGenTextures(1, &_ID);
    glBindTexture(GL_TEXTURE_2D, _ID);
    // Set texture wraping/filtering options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // Use image data as texture
    if (data)
    {
        GLenum format;
        if (imChannels == 1)
            format = GL_RED;
        else if (imChannels == 3)
            format = GL_RGB;
        else
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, (GLint)format, imWidth, imHeight, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture image data:" << stbi_failure_reason() << std::endl;
    }
    stbi_image_free(data);
}

#endif