#ifndef MODEL_HPP
#define MODEL_HPP

#include <vector>

#include "Engine/shader.hpp"
#include "Engine/mesh.hpp"

class Model
{
public:
    virtual void draw(Shader& shader, glm::mat4 model);
protected:
    std::vector<Mesh> _meshes;
};

void Model::draw(Shader& shader, glm::mat4 model)
{
    for (auto & _mesh : _meshes)
        _mesh.draw(shader);
}

#endif