#ifndef MESH_CLASS_H
#define MESH_CLASS_H

#include <string>

#include "shaderVertexArray/2dVAO.h"
#include "shaderBuffers/2dVBO.h"
#include "shaderBuffers/EBO.h"
#include "camera/2dcameraClass.h"
#include "textures/textureClass.h"
//#include "worldObjects/obstacles/obstacles.h"

class Mesh
{
    public:
        std::vector <Vertex> vertices;
        std::vector <GLuint> indices;
        std::vector <Texture> textures;
        VAO VAO;
        VBO VBO;

        Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures);

        void Draw(Shader& shader, Camera& camera);
        void updateVertices(std::vector <Vertex> verts);
};

#endif
