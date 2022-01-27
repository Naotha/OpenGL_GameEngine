#ifndef MODEL_LOADER_HPP
#define MODEL_LOADER_HPP

#include "Engine/model.hpp"
#include "Engine/modelDefault.hpp"
#include "Engine/modelGLTF.hpp"

class ModelLoader
{
public:
    static Model* LoadModel(const std::string& path)
    {
        std::string format = path.substr(path.size() - 4);
        if (format == "gltf")
        {
            return new ModelGLTF(path);
        }
        else
        {
            return new ModelDefault(path);
        }
    }
};

#endif