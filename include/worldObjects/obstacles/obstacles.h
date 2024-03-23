#ifndef OBSTACLE_CLASS_H
#define OBSTACLE_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mesh/2d_mesh_class.h"

class Pipe
{
    public:
        Pipe(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures);
        void Draw(Shader& shader, Camera& camera);
        void updatePosition();
        void check_on_screen();

        std::vector <Vertex> Vertices;
        bool on_screen = false;
        float scroll_speed = -0.0001f;

    private:
        std::vector <GLuint> Indices;
        std::vector <Texture> Textures;
        std::vector <Mesh> meshes;
};

#endif
