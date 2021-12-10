#pragma once

#include <string>
#include <vector>
#include <iostream>
#include "glm/glm.hpp"

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Engine/shader.hpp"
#include "Engine/mesh.hpp"
#include "Engine/texture.hpp"
#include "Engine/model.hpp"

class ModelDefault : public Model
{
public:
    ModelDefault(const std::string path);

private:
    std::vector<Texture> _loadedTextures;
    std::string _directory;

    void _loadModelDefault(std::string path);
    void _processNode(aiNode* node, const aiScene* scene);
    Mesh _processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture> _loadMaterialTextures(aiMaterial* material, aiTextureType type, TextureType typeName);
};

ModelDefault::ModelDefault(const std::string path)
{
    _loadModelDefault(path);
}

void ModelDefault::_loadModelDefault(std::string path)
{
    
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }
    _directory = path.substr(0, path.find_last_of('/'));
    
    _processNode(scene->mRootNode, scene);
}

void ModelDefault::_processNode(aiNode* node, const aiScene* scene)
{
    // Process all Meshes in Node
    for (int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        _meshes.push_back(_processMesh(mesh, scene));
    }
    // Process all Children Nodes in Node
    for (int i = 0; i < node->mNumChildren; i++)
    {
        _processNode(node->mChildren[i], scene);
    }
}

Mesh ModelDefault::_processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    // Vertices
    for (int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector;
        // Position
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;
        // Normal
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
        }
        // Texture Coordinates
        if (mesh->mTextureCoords[0])
        {
            glm::vec2 vector;
            vector.x = mesh->mTextureCoords[0][i].x;
            vector.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoord = vector;
        }
        else
        {
            vertex.texCoord = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }
    // Indices
    for (int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // Materials
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
        // Diffuse
        std::vector<Texture> diffuseMaps = _loadMaterialTextures(material, aiTextureType_DIFFUSE, TextureType::DIFFUSE);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // Specular
        std::vector<Texture> specularMaps = _loadMaterialTextures(material, aiTextureType_SPECULAR, TextureType::SPECULAR);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }

    return Mesh(vertices, indices, textures);
}

std::vector<Texture> ModelDefault::_loadMaterialTextures(aiMaterial* material, aiTextureType type, TextureType typeName)
{
    std::vector<Texture> textures;
    int count = 0;
    for (int i = 0; i < material->GetTextureCount(type); i++)
    {
        aiString str;
        material->GetTexture(type, i, &str);
        std::string fileName = std::string(str.C_Str());
        std::string filePath = _directory + "/" + fileName;
        bool skip = false;
        for (int j = 0; j < _loadedTextures.size(); j++)
        {   
            // Check if texture has been loaded yet
            if (std::strcmp(_loadedTextures[j].getPath(), filePath.c_str()) == 0 )
            {
                textures.push_back(_loadedTextures[j]);
                skip = true;
                break;
            }
        }
        // Load texture if it isn't loaded already
        if (!skip)
        {
            Texture texture(filePath.c_str(), typeName);
            textures.push_back(texture);
            _loadedTextures.push_back(texture);
        }
    }
    return textures;
}
