#include "mesh/2d_mesh_class.h"

Mesh::Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures)
{
    Mesh::vertices = vertices;
    Mesh::indices = indices;
    Mesh::textures = textures;

    VAO.Bind();
    //Generates vertex BUFFER object and links it to defined vertices
    VBO.Construct(vertices);
    //EBO = Element (a collection of indices) buffer object
    //Generates ELEMENT buffer object and links it to defined indices
    EBO EBO(indices);

    //Link VBO attributes to VAO 
    VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
    //Offset is 3 float numbers in bytes for color section of our vertex array
    VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
    //Ofset 6 for texture coordinates          
    VAO.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));

    //Unbind all to prevent accidentally modifying them
    VAO.Unbind();
    VBO.Unbind();
    EBO.Unbind();
}

void Mesh::Draw(Shader& shader, Camera& camera)
{
    shader.Activate();
    VAO.Bind();

    unsigned int numDiffuse = 0;
    unsigned int numSpecular = 0;
    
    if (textures[0].type == "NULL")
    {
        std::cout <<"--- NULL TEXTURE ---\n";
        glUniform3f(glGetUniformLocation(shader.ID, "cameraPosition"), camera.Position.x, camera.Position.y, camera.Position.z);
        camera.Matrix(shader, "cameraMatrix");
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    }
    else
    {
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            std::string num;
            std::string type = textures[i].type;
            if (type == "diffuse")
            {
                num = std::to_string(numDiffuse++);
            }
            else if (type == "specular")
            {
                num = std::to_string(numSpecular++);
            }
            //std::cout << "---" << type + num << "---" << std::endl;
            textures[i].texUnit(shader, (type + num).c_str(), i);
            textures[i].Bind();
            glUniform3f(glGetUniformLocation(shader.ID, "cameraPosition"), camera.Position.x, camera.Position.y, camera.Position.z);
            camera.Matrix(shader, "cameraMatrix");
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        }
    }
    VAO.Unbind();
}

void Mesh::updateVertices(std::vector <Vertex> Vertices)
{
    vertices = Vertices;
    VBO.Bind();
    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), &vertices[0]);
    VBO.Unbind();
    /*
    for (int i = 0; i < Vertices.size(); i++)
    {
        Mesh::vertices[i].position.x = Vertices[i].position.x;
        Mesh::vertices[i].position.y = Vertices[i].position.y;
        Mesh::vertices[i].position.z = Vertices[i].position.z;
    }
    */
}