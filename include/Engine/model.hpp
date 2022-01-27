#ifndef MODEL_HPP
#define MODEL_HPP

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
    for (auto & _mesh : _meshes)
        _mesh.draw(shader);
}

#endif