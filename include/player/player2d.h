#ifndef PLAYER_CLASS_H
#define PLAYER_CLASS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "mesh/2d_mesh_class.h"

class Player
{
    public:
        Player(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures);
        void Draw(Shader& shader, Camera& camera);
        void Inputs(GLFWwindow* window);
        bool collision_detection(float player_x, float player_y, std::vector <Vertex>& obj_vertices);

        void reset_score();
        void reset_position(float widthAdjustment);

        std::vector <Mesh> player_meshes;
        std::vector <Vertex> Vertices;
        float Speed = 0.0f;
        double FallTime = 0.0f;
        bool player_colliding = false;
        float score = 0;
        bool new_focus = false;
        bool player_off_screen = false;
        
    private:
        const float Gravity = -0.0012f;
        bool first_press = true;
        
        std::vector <GLuint> Indices;
        std::vector <Texture> Textures;
        
};

#endif
