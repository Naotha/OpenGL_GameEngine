#include "Engine/model.hpp"
#include "Engine/modelOBJ.hpp"
#include "Engine/modelGLTF.hpp"

class ModelLoader
{
public:
    static Model* LoadModel(std::string path)
    {
        std::string format = path.substr(path.size() - 3);
        if (format == "obj")
        {
            return new ModelOBJ(path);
        }

        format = path.substr(path.size() - 4);
        if (format == "gltf")
        {
            return new ModelGLTF(path);
        }

        std::cout << "Failed to load model data: " << "Unsopported file format" << std::endl;

        return nullptr;
    }
};