#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <algorithm>
#include "glm/glm.hpp"

#include "json/json.hpp"

#include "Engine/shader.hpp"
#include "Engine/mesh.hpp"
#include "Engine/texture.hpp"
#include "Engine/model.hpp"

class ModelGLTF : public Model
{
public:
    ModelGLTF(const std::string path);
    void draw(Shader& shader) override
    {
        for (int i = 0; i < _meshes.size(); i++)
        {   
            // shader.bind();
            // //TODO Make better transformation system
            // glm::mat4 modelIT = glm::transpose(glm::inverse(_transformations[i]));
            // shader.setUniformMat4("u_model", _transformations[i]);
            // shader.setUniformMat4("u_modelIT", modelIT);
            // shader.unbind();
            _meshes[i].draw(shader);
        }
    }
    
private:
    std::string _path;
    std::string _directory;
    std::vector<Texture> _loadedTextures;
    std::vector<unsigned char> _modelData;
    std::vector<glm::mat4> _transformations;
    nlohmann::json _JSON;


    void _loadModelGLTF();
    void _processNode(unsigned int child, glm::mat4 matrix = glm::mat4(1.0f));
    void _loadMesh(unsigned int meshIndex);

    std::string _readInFile(std::string path);
    std::vector<unsigned char> _getData();

    std::vector<float> _getFloats(nlohmann::json accessor);
    std::vector<unsigned int> _getIndices(nlohmann::json accessor);

    std::vector<glm::vec2> _groupFloatsVec2(std::vector<float> floats);
    std::vector<glm::vec3> _groupFloatsVec3(std::vector<float> floats);
    std::vector<glm::vec4> _groupFloatsVec4(std::vector<float> floats);

    std::vector<Vertex> _assembleVertices(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<glm::vec2> texCoords);
    std::vector<Texture> _getTextures();
};

ModelGLTF::ModelGLTF(const std::string path)
{
    _path = path;
	_directory = _path.substr(0, _path.find_last_of('/') + 1);
    _loadModelGLTF();
}

void ModelGLTF::_loadModelGLTF()
{
    //Read in file contents
    std::string modelFileContent = _readInFile(_path);
    _JSON = nlohmann::json::parse(modelFileContent);
    _modelData = _getData();

    _processNode(0);
}

std::string ModelGLTF::_readInFile(std::string path)
{
    std::string fileContent;
    std::ifstream file(path, std::ios::binary);
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();
        fileContent = stream.str();
    }
    catch(const std::exception& e)
    {
        std::cout << "ERROR::MODEL::FILE_NOT_SUCCESFULLY_READ " << e.what() <<std::endl;
    }

    return fileContent;
}

std::vector<unsigned char> ModelGLTF::_getData()
{
    std::string bytes;
    std::string uri = _JSON["buffers"][0]["uri"];

    bytes = _readInFile(_directory + uri);

    std::vector<unsigned char> data(bytes.begin(), bytes.end());
    return data;
}

void ModelGLTF::_processNode(unsigned int child, glm::mat4 matrix)
{
    nlohmann::json node = _JSON["nodes"][child];

    // Get translation
    glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
    if (node.find("translation") != node.end())
    {
       float translationValues[3] = {node["translation"][0], node["translation"][1], node["translation"][2]}; 
        translation = glm::make_vec3(translationValues);
    }

    // Get quaternion
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    if (node.find("rotation") != node.end())
    {
        float rotationValues[4] = {node["rotation"][3], node["rotation"][0], node["rotation"][1], node["rotation"][2]};
        rotation = glm::make_quat(rotationValues);
    }

    // Get scale
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    if (node.find("scale") != node.end())
    {
        float scaleValues[3] = {node["scale"][0], node["scale"][1], node["scale"][2]};
        scale = glm::make_vec3(scaleValues);
    }

    // Get matrix
    glm::mat4 mat = glm::mat4(1.0f);
    if (node.find("matrix") != node.end())
    {
        float matValues[16];
        for (unsigned int i = 0; i < node["matrix"].size(); i++)
            matValues[i] = node["matrix"][i];
		mat = glm::make_mat4(matValues);
    }

    // Calculate transformation
    glm::mat4 trans = glm::translate(glm::mat4(1.0f), translation);
    glm::mat4 rot = glm::mat4_cast(rotation);
    glm::mat4 sc = glm::scale(glm::mat4(1.0f), scale);
    
    glm::mat4 transformation = matrix * mat * trans * rot * sc;
    //TODO Something is wrong with rotation, figure out why
    transformation = glm::rotate(transformation, glm::radians(90.0f), glm::vec3(0, 0, 1));

    // Get mesh
    if (node.find("mesh") != node.end())
    {   
        _transformations.push_back(transformation);
        _loadMesh(node["mesh"]);
    }

    // Get child
    if (node.find("children") != node.end())
    {
        for (unsigned int i = 0; i < node["children"].size(); i++)
            _processNode(node["children"][i], transformation);
    }
}

void ModelGLTF::_loadMesh(unsigned int meshIndex)
{
    // Get accessor indices
    unsigned int positionInd = _JSON["meshes"][meshIndex]["primitives"][0]["attributes"]["POSITION"];
    unsigned int normalInd = _JSON["meshes"][meshIndex]["primitives"][0]["attributes"]["NORMAL"];
    unsigned int texCoordInd = _JSON["meshes"][meshIndex]["primitives"][0]["attributes"]["TEXCOORD_0"];
    unsigned int indicesInd = _JSON["meshes"][meshIndex]["primitives"][0]["indices"];
    // Get vertex components
    std::vector<float> positionValues = _getFloats(_JSON["accessors"][positionInd]);
    std::vector<float> normalValues = _getFloats(_JSON["accessors"][normalInd]);
    std::vector<float> texCoordValues = _getFloats(_JSON["accessors"][texCoordInd]);
    std::vector<glm::vec3> positions = _groupFloatsVec3(positionValues);
    std::vector<glm::vec3> normals = _groupFloatsVec3(normalValues);
    std::vector<glm::vec2> texCoords = _groupFloatsVec2(texCoordValues);

    // Get mesh components
    std::vector<Vertex> vertices = _assembleVertices(positions, normals, texCoords);
    std::vector<unsigned int> indices = _getIndices(_JSON["accessors"][indicesInd]);
    std::vector<Texture> textures = _getTextures();

    _meshes.push_back(Mesh(vertices, indices, textures));
}

std::vector<float> ModelGLTF::_getFloats(nlohmann::json accessor)
{
    std::vector<float> floats;

    // Get properties from accessor
    unsigned int bufferViewInd = accessor.value("bufferView", 1);
    unsigned int count = accessor["count"];
    unsigned int accByteOffset = accessor.value("byteOffset", 0);
    std::string type = accessor["type"];

    // Get bufferView properties
    nlohmann::json bufferView = _JSON["bufferViews"][bufferViewInd];
    unsigned int byteOffset = bufferView["byteOffset"];

    // Interpret type
    unsigned int numOfFloats;
    if      (type == "SCALAR")  numOfFloats = 1;
    else if (type == "VEC2")    numOfFloats = 2;
    else if (type == "VEC3")    numOfFloats = 3;
    else if (type == "VEC4")    numOfFloats = 4;
    else throw std::invalid_argument("ERROR::MODEL::INVALID_TYPE");

    // Get float data from bytes
    unsigned int dataBegin = byteOffset + accByteOffset;
    unsigned int dataLength = count * 4 * numOfFloats;
    for (unsigned int i = dataBegin; i < dataBegin + dataLength; i)
    {
        unsigned char bytes[] = { _modelData[i++], _modelData[i++], _modelData[i++], _modelData[i++]};
        float value;
        std::memcpy(&value, bytes, sizeof(float));
        floats.push_back(value);
    }

    return floats;
}

std::vector<unsigned int> ModelGLTF::_getIndices(nlohmann::json accessor)
{
    std::vector<unsigned int> indices;

    // Get properties from accessor
    unsigned int bufferViewInd = accessor.value("bufferView", 0);
    unsigned int count = accessor["count"];
    unsigned int accByteOffset = accessor.value("byteOffset", 0);
    unsigned int componentType = accessor["componentType"];

    // Get bufferView properties
    nlohmann::json bufferView = _JSON["bufferViews"][bufferViewInd];
    unsigned int byteOffset = bufferView["byteOffset"];

    // Get index data from bytes
    unsigned int dataBegin = byteOffset + accByteOffset;
    if (componentType == 5125) // Unsigned int
    {
        for (unsigned int i = dataBegin; i < dataBegin + count * 4; i)
        {
            unsigned char bytes[] = { _modelData[i++], _modelData[i++], _modelData[i++], _modelData[i++]};
            unsigned int value;
            std::memcpy(&value, bytes, sizeof(unsigned int));
            indices.push_back((unsigned int)value);
        }
    }
    else if (componentType == 5123) // Unsigned short
    {
        for (unsigned int i = dataBegin; i < dataBegin + count * 2; i)
        {
            unsigned char bytes[] = { _modelData[i++], _modelData[i++]};
            unsigned short value;
            std::memcpy(&value, bytes, sizeof(unsigned short));
            indices.push_back((unsigned int)value);
        }
    }
    else if (componentType == 5122) // Short
    {
        for (unsigned int i = dataBegin; i < dataBegin + count * 2; i)
        {
            unsigned char bytes[] = { _modelData[i++], _modelData[i++]};
            short value;
            std::memcpy(&value, bytes, sizeof(short));
            indices.push_back((unsigned int)value);
        }
    }
    std::reverse(indices.begin(), indices.end());
    return indices;
}

std::vector<glm::vec2> ModelGLTF::_groupFloatsVec2(std::vector<float> floats)
{
    std::vector<glm::vec2> vecs;
    for (int i = 0; i < floats.size(); i)
        vecs.push_back(glm::mat2(0.0f, -1.0f, 1.0f, 0.0f) * glm::vec2(floats[i++], floats[i++]));
    return vecs;
}

std::vector<glm::vec3> ModelGLTF::_groupFloatsVec3(std::vector<float> floats)
{
    std::vector<glm::vec3> vecs;
    for (int i = 0; i < floats.size(); i)
        vecs.push_back(glm::vec3(floats[i++], floats[i++], floats[i++]));
    return vecs;
}

std::vector<glm::vec4> ModelGLTF::_groupFloatsVec4(std::vector<float> floats)
{
    std::vector<glm::vec4> vecs;
    for (int i = 0; i < floats.size(); i)
        vecs.push_back(glm::vec4(floats[i++], floats[i++], floats[i++], floats[i++]));
    return vecs;
}

std::vector<Vertex> ModelGLTF::_assembleVertices(std::vector<glm::vec3> positions, std::vector<glm::vec3> normals, std::vector<glm::vec2> texCoords)
{
    std::vector<Vertex> vertices;
    for (int i = 0; i < positions.size(); i++)
    {
        Vertex vertex{positions[i], normals[i], texCoords[i]};
        vertices.push_back(vertex);
    }
    return vertices;
}

std::vector<Texture> ModelGLTF::_getTextures()
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < _JSON["images"].size(); i++)
    {   
        std::string textureURI = _JSON["images"][i]["uri"];
        std::string filePath = _directory + textureURI;
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

        if (!skip)
        {
            if (textureURI.find("baseColor") != std::string::npos)
            {
                Texture texture = Texture(filePath.c_str(), TextureType::DIFFUSE);
                textures.push_back(texture);
                _loadedTextures.push_back(texture);
            }
            else if (textureURI.find("metallicRoughness") != std::string::npos)
            {
                Texture texture = Texture(filePath.c_str(), TextureType::SPECULAR);
                textures.push_back(texture);
                _loadedTextures.push_back(texture);
            }
        }
    }

    return textures;
}
