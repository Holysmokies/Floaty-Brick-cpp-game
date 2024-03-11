#include "model/2dmodels.h"

Model::Model(const char* file)
{
    std::string text = get_file_contents(file);
    JSON = json::parse(text);

    Model::file = file;
    data = getData();

    traverseNode(0);
}

void Model::Draw(Shader& shader, Camera& camera)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i].Mesh::Draw(shader, camera);
    }
}

void Model::loadMesh(unsigned int meshIndex)
{
    unsigned int positionAccessorIndex = JSON["meshes"][meshIndex]["primitives"][0]["attributes"]["POSITION"];
    unsigned int normalAccessorIndex = JSON["meshes"][meshIndex]["primitives"][0]["attributes"]["NORMAL"];
    unsigned int textureAccessorIndex = JSON["meshes"][meshIndex]["primitives"][0]["attributes"]["TEXCOORD_0"];
    unsigned int indicesAccessorIndex = JSON["meshes"][meshIndex]["primitives"][0]["indices"];

    std::vector <float> positionVec = getFloats(JSON["accessors"][positionAccessorIndex]);
    std::vector <glm::vec3> positions = groupFloatsVec3(positionVec);
    std::vector <float> normalVec = getFloats(JSON["accessors"][normalAccessorIndex]);
    std::vector <glm::vec3> normals = Model::groupFloatsVec3(normalVec);
    std::vector <float> textureVec = getFloats(JSON["accessors"][textureAccessorIndex]);
    std::vector <glm::vec2> texUVs = groupFloatsVec2(textureVec);

    std::vector <Vertex> vertices = assembleVertices(positions, texUVs);
    std::vector <GLuint> indices = getIndices(JSON["accessors"][indicesAccessorIndex]);
    std::vector <Texture> textures = getTextures();

    meshes.push_back(Mesh(vertices, indices, textures));
}

void Model::traverseNode(unsigned int nodeIndex, glm::mat4 matrix)
{
    json node = JSON["nodes"][nodeIndex];

    //MAKE TRANSLATION VECTOR -------------------------------
    glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
    if (node.find("translation") != node.end())
    {
        float translationValues[3];
        for (unsigned int i = 0; i < node["translation"].size(); i++)
        {
            translationValues[i] = (node["translation"][i]);
        }
        translation = glm::make_vec3(translationValues);
    }
    //MAKE ROTATION QUATERNION -------------------------------
    glm::quat rotation = glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
    if (node.find("rotation") != node.end())
    {
        float rotationValues[4] = 
        {   
            // 3,0,1,2 because GLTF uses XYZW and GLM uses WXYZ for quaternions (quat)
            // so we must manually change how the data is stored for use
            node["rotation"][3],
            node["rotation"][0],
            node["rotation"][1],
            node["rotation"][2],
        };
        rotation = glm::make_quat(rotationValues);
    }
    //MAKE SCALE VECTOR ------------------------------------
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    if (node.find("scale") != node.end())
    {
        float scaleValues[3];
        for (unsigned int i = 0; i < node["scale"].size(); i++)
        {
            scaleValues[i] = node["scale"][i];
        }
        scale = glm::make_vec3(scaleValues);
    }
    //MAKE MATRIX ------------------------------------------
    glm::mat4 matrixNode = glm::mat4(1.0f);
    if (node.find("matrix") != node.end())
    {
        float matrixValues[16];
        for (unsigned int i = 0; i < node["matrix"].size(); i++)
        {
            matrixValues[i] = node["matrix"][i];
        }
        matrixNode = glm::make_mat4(matrixValues);
    }

    glm::mat4 trans = glm::mat4(1.0f);
    glm::mat4 rot = glm::mat4(1.0f);
    glm::mat4 sca = glm::mat4(1.0f);

    trans = glm::translate(trans, translation);
    rot = glm::mat4_cast(rotation);
    sca = glm::scale(sca, scale);

    glm::mat4 matrixNextNode = matrix * matrixNode * trans * rot * sca;

    if (node.find("mesh") != node.end())
    {
        translationsMeshes.push_back(translation);
        rotationsMeshes.push_back(rotation);
        scalesMeshes.push_back(scale);
        matricesMeshes.push_back(matrixNextNode);

        loadMesh(node["mesh"]);
    }

    //Recursively pushes current matrix to each 'child' of node to update them
    if (node.find("children") != node.end())
    {
        for (unsigned int i = 0; i < node["children"].size(); i++)
        {
            traverseNode(node["children"][i], matrixNextNode);
        }
    }
}

std::vector <unsigned char> Model::getData()
{
    std::string bytesText;
    //"uri" give name of a bin (binary) data file
    std::string uri = JSON["buffers"][0]["uri"];

    std::string fileStr = std::string(file);
    std::string fileDirectory = fileStr.substr(0,fileStr.find_last_of('/') + 1);
    bytesText = get_file_contents((fileDirectory + uri).c_str());

    std::vector <unsigned char> data(bytesText.begin(), bytesText.end());
    return data;
}

std::vector <float> Model::getFloats(json accessor)
{
    std::vector <float> floatVec;
    unsigned int buffViewInd = accessor.value("bufferView", 1);
    unsigned int count = accessor["count"];
    unsigned int accByteOffset = accessor.value("byteOffset", 0);
    std::string type = accessor["type"];

    json bufferView = JSON["bufferViews"][buffViewInd];
    unsigned int byteOffset = bufferView["byteOffset"];

    unsigned int numPerVert;
    if (type == "SCALAR") numPerVert = 1;
    else if (type == "VEC2") numPerVert = 2;
    else if (type == "VEC3") numPerVert = 3;
    else if (type == "VEC4") numPerVert = 4;
    else throw std::invalid_argument("Invalid Type (not scalar, vec2, vec3, vec4)");

    unsigned int begOfData = byteOffset + accByteOffset;
    unsigned int lenOfData = count * 4 * numPerVert;
    for (unsigned int i = begOfData; i < begOfData + lenOfData; i)
    {
        unsigned char bytes[] = {data[i++], data[i++], data[i++], data[i++]};
        float value;
        std::memcpy(&value, bytes, sizeof(float));
        floatVec.push_back(value);
    }
    return floatVec;
};

std::vector <GLuint> Model::getIndices(json accessor)
{
    std::vector <GLuint> indices;
    unsigned int buffViewInd = accessor.value("bufferView", 0);
    unsigned int count = accessor["count"];
    unsigned int accByteOffset = accessor.value("byteOffset", 0);
    unsigned int componentType = accessor["componentType"];

    json bufferView = JSON["bufferViews"][buffViewInd];
    unsigned int byteOffset = bufferView["byteOffset"];

    unsigned int begOfData = byteOffset + accByteOffset;
    if (componentType == 5125)
    {
        for (unsigned int i = begOfData; i < byteOffset + accByteOffset + count * 4; i)
        {
            unsigned char bytes[] = {data[i++], data[i++], data[i++], data[i++]};
            unsigned int value;
            std::memcpy(&value, bytes, sizeof(unsigned int));
            indices.push_back((GLuint)value);
        }
    }
    else if (componentType == 5123)
    {
        for (unsigned int i = begOfData; i < byteOffset + accByteOffset + count * 2; i)
        {
            unsigned char bytes[] = {data[i++], data[i++]};
            unsigned short value;
            std::memcpy(&value, bytes, sizeof(unsigned short));
            indices.push_back((GLuint)value);
        }
    }
    else if (componentType == 5122)
    {
        for (unsigned int i = begOfData; i < byteOffset + accByteOffset + count * 2; i)
        {
            unsigned char bytes[] = {data[i++], data[i++]};
            short value;
            std::memcpy(&value, bytes, sizeof(short));
            indices.push_back((GLuint)value);
        }
    }
    return indices;
};

std::vector <Texture> Model::getTextures()
{
    std::vector <Texture> textures;

    std::string fileStr = std::string(file);
    std::string fileDirectory = fileStr.substr(0, fileStr.find_last_of('/') + 1);

    unsigned int unit = 0;
    for (unsigned int i = 0; i < JSON["images"].size(); i++)
    {
        std::string texturePath = JSON["images"][i]["uri"];

        bool skip = false;
        for (unsigned int j = 0; j < loadedTextureNames.size(); j++)
        {
            if (loadedTextureNames[j] == texturePath)
            {
                textures.push_back(loadedTextures[j]);
                skip = true;
                break;
            }
        }
        if (!skip)
        {
            if (texturePath.find("baseColor") != std::string::npos)
            {
                Texture diffuse = Texture((fileDirectory + texturePath).c_str(), "diffuse", loadedTextures.size());
                textures.push_back(diffuse);
                loadedTextures.push_back(diffuse);
                loadedTextureNames.push_back(texturePath);
            }
            else if (texturePath.find("metallicRoughness") != std::string::npos)
            {
                Texture specular = Texture((fileDirectory + texturePath).c_str(), "specular", loadedTextures.size());
                textures.push_back(specular);
                loadedTextures.push_back(specular);
                loadedTextureNames.push_back(texturePath);

            }
        }   
    }  
    return textures; 
}

std::vector <Vertex> Model::assembleVertices(std::vector <glm::vec3> positions,
                                             std::vector <glm::vec2> textureCoordinates)
{
    std::vector <Vertex> vertices;
    for (int i = 0; i < positions.size(); i++)
    {
        vertices.push_back
        (
            Vertex {positions[i], glm::vec3(1.0f, 1.0f, 1.0f), textureCoordinates[i]}
        );
    }
    return vertices;
};

std::vector <glm::vec2> Model::groupFloatsVec2(std::vector <float> floatVec)
{
    std::vector <glm::vec2> vectors;
    for (int i = 0; i < floatVec.size(); i)
    {
        vectors.push_back(glm::vec2(floatVec[i++], floatVec[i++]));
    }
    return vectors;
};

std::vector <glm::vec3> Model::groupFloatsVec3(std::vector <float> floatVec)
{
    std::vector <glm::vec3> vectors;
    for (int i = 0; i < floatVec.size(); i)
    {
        vectors.push_back(glm::vec3(floatVec[i++], floatVec[i++], floatVec[i++]));
    }
    return vectors;
};

std::vector <glm::vec4> Model::groupFloatsVec4(std::vector <float> floatVec)
{
    std::vector <glm::vec4> vectors;
    for (int i = 0; i < floatVec.size(); i)
    {
        vectors.push_back(glm::vec4(floatVec[i++], floatVec[i++], floatVec[i++], floatVec[i++]));
    }
    return vectors;
};

