#pragma once

#include <vector>

#include "Engine/shader.hpp"
#include "Engine/mesh.hpp"

class Model
{
public:
    virtual void draw(Shader& shader);
protected:
    std::vector<Mesh> _meshes;
};

void Model::draw(Shader& shader)
{
    for (int i = 0; i < _meshes.size(); i++)
        _meshes[i].draw(shader);
}