#include "mesh/3d_mesh_class.h"

Mesh::Mesh(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures)
{
    Mesh::vertices = vertices;
    Mesh::indices = indices;
    Mesh::textures = textures;

    VAO.Bind();
    //Generates vertex BUFFER object and links it to defined vertices
    VBO VBO(vertices);
    //EBO = Element (a collection of indices) buffer object
    //Generates ELEMENT buffer object and links it to defined indices
    EBO EBO(indices);

    //Link VBO attributes to VAO 
    VAO.LinkAttrib(VBO, 0, 3, GL_FLOAT, sizeof(Vertex), (void*)0);
    //Offset is 3 float numbers in bytessizeof(Vertex) section of our vertex array
    VAO.LinkAttrib(VBO, 1, 3, GL_FLOAT, sizeof(Vertex), (void*)(3 * sizeof(float)));
    //Ofset 6 for texture coordinate mapsizeof(Vertex)          v
    VAO.LinkAttrib(VBO, 2, 3, GL_FLOAT, sizeof(Vertex), (void*)(6 * sizeof(float)));
    //Offset 8 for normals
    VAO.LinkAttrib(VBO, 3, 2, GL_FLOAT, sizeof(Vertex), (void*)(9 * sizeof(float)));

    //Unbind all to prevent accidentally modifying them
    VAO.Unbind();
    VBO.Unbind();
    EBO.Unbind();
}

void Mesh::Draw
(
    Shader& shader,
    Camera& camera,
    glm::mat4 matrix,
    glm::vec3 translation,
    glm::quat rotation,
    glm::vec3 scale  
)
{
    shader.Activate();
    VAO.Bind();

    unsigned int numDiffuse = 0;
    unsigned int numSpecular = 0;
    if (textures[0].type == NULL)
    {
         glUniform3f(glGetUniformLocation(shader.ID, "cameraPosition"), camera.Position.x, camera.Position.y, camera.Position.z);
            camera.Matrix(shader, "cameraMatrix");

            glm::mat4 trans = glm::mat4(1.0f);
            glm::mat4 rot = glm::mat4(1.0f);
            glm::mat4 sca = glm::mat4(1.0f);

            trans = glm::translate(trans, translation);
            rot = glm::mat4_cast(rotation);
            sca = glm::scale(sca, scale);

            glUniformMatrix4fv(glGetUniformLocation(shader.ID, "translation"), 1, GL_FALSE, glm::value_ptr(trans));
            glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotation"), 1, GL_FALSE, glm::value_ptr(rot));
            glUniformMatrix4fv(glGetUniformLocation(shader.ID, "scale"), 1, GL_FALSE, glm::value_ptr(sca));
            glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(matrix));

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

            glm::mat4 trans = glm::mat4(1.0f);
            glm::mat4 rot = glm::mat4(1.0f);
            glm::mat4 sca = glm::mat4(1.0f);

            trans = glm::translate(trans, translation);
            rot = glm::mat4_cast(rotation);
            sca = glm::scale(sca, scale);

            glUniformMatrix4fv(glGetUniformLocation(shader.ID, "translation"), 1, GL_FALSE, glm::value_ptr(trans));
            glUniformMatrix4fv(glGetUniformLocation(shader.ID, "rotation"), 1, GL_FALSE, glm::value_ptr(rot));
            glUniformMatrix4fv(glGetUniformLocation(shader.ID, "scale"), 1, GL_FALSE, glm::value_ptr(sca));
            glUniformMatrix4fv(glGetUniformLocation(shader.ID, "model"), 1, GL_FALSE, glm::value_ptr(matrix));

            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        }
    }
    
    //std::cout << "------FINISH------" << std::endl;
} 