#ifndef BUTTON_CLASS_H
#define BUTTON_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mesh/2d_mesh_class.h"

class Button
{
    public:
        Button(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures);

        void Draw(Shader& shader, Camera& camera);
        bool interact(GLFWwindow* window, int width, int height);

        std::vector <Mesh> button_mesh;

    private:
        std::vector <Vertex> Vertices;
        std::vector <GLuint> Indices;
        std::vector <Texture> Textures;

        bool mouseover = false;
        bool pressed = false;
};

#endif
