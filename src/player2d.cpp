#include "player/player2d.h"

Player::Player(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures)
{
    Vertices = vertices; 
    Indices = indices;
    Textures = textures;
    player.push_back(Mesh(Vertices, Indices, Textures));
}

void Player::Draw(Shader& shader, Camera& camera)
{
    for (int i = 0; i < player.size(); i++)
    {   
        player[i].updateVertices(Vertices);
        player[i].Draw(shader, camera);
    }
}

void Player::Inputs(GLFWwindow* window)
{
    double old_time = glfwGetTime();
    double new_time;
    double fall_speed = Gravity * FallTime * FallTime;

    //IF FIRST KEY PRESS FOR SPACE/JUMP AND FALLING
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && first_press && Speed + Gravity * FallTime * FallTime < -0.00001f)
    {   
        FallTime = 0.0;
        Speed = 0.00055f;

        for (int i = 0; i < Player::Vertices.size(); i++)
        {   
            Player::Vertices[i].position.y += Speed;
        }

        first_press = false;
    }

    //IF FIRST KEY PRESS FOR SPACE/JUMP AND NOT FALLING
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && first_press && Speed + fall_speed > -0.00001f)
        {   
            if (Speed > 0.0f)
            {
                Speed += fall_speed / 10;
                for (int i = 0; i < Player::Vertices.size(); i++)
                {   
                    Player::Vertices[i].position.y += Speed + fall_speed;
                }
            }
            else
            {
                for (int i = 0; i < Player::Vertices.size(); i++)
                {   
                    Player::Vertices[i].position.y += Speed + fall_speed;
                }
            }

            new_time = glfwGetTime();
            FallTime += 1000 * (new_time - old_time);
            first_press = false;
        }

    //IF HOLDING SPACE DOWN
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !first_press)
    {   
        if (Speed > 0.0f)
        {   
            Speed += fall_speed / 10;
            for (int i = 0; i < Player::Vertices.size(); i++)
            {   
                Player::Vertices[i].position.y += Speed + fall_speed;
            }
        }
        else
        {
            for (int i = 0; i < Player::Vertices.size(); i++)
            {   
                Player::Vertices[i].position.y += Speed + fall_speed;
            }
        }

        new_time = glfwGetTime();
        FallTime += 1000 * (new_time - old_time);
    }

    //IF NOT JUMPING/PRESSING SPACE
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
    {
        if (Speed > 0.0f)
        {
            Speed += fall_speed / 10;
            for (int i = 0; i < Player::Vertices.size(); i++)
            {   
                Player::Vertices[i].position.y += Speed + fall_speed;
            }
        }
        else
        {
            for (int i = 0; i < Player::Vertices.size(); i++)
            {   
                Player::Vertices[i].position.y += Speed + fall_speed;
            }
        }

        new_time = glfwGetTime();
        FallTime += 1000 * (new_time - old_time);
        first_press = true;
    }
}