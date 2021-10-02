#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

class Texture
{
public:
    Texture(const char* path, const char* uniformName, unsigned int texColorChannnels);

    unsigned int getID();
    const char* getUniformName();
    const char* getPath();
private:
    unsigned int _ID;
    const char* _uniformName;
    const char* _path;
};

Texture::Texture(const char* path, const char* uniformName, unsigned int texColorChannels = GL_RGB)
{
    _path = path;
    _uniformName = uniformName;
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
        glTexImage2D(GL_TEXTURE_2D, 0, texColorChannels, imWidth, imHeight, 0, texColorChannels, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture image data:" << stbi_failure_reason() << std::endl;
    }
    stbi_image_free(data);
}

unsigned int Texture::getID()
{
    return _ID;
}

const char* Texture::getUniformName()
{
    return _uniformName;
}

const char* Texture::getPath()
{
    return _path;
}

#endif