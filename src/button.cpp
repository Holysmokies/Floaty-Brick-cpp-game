#include "buttons/button.h"


Button::Button(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures)
{
    //make variables for use in other functions
    Vertices = vertices;
    Indices = indices;
    Textures = textures;
    //create mesh for button object
    button_mesh.push_back(Mesh(Vertices, Indices, Textures));
}

void Button::Draw(Shader& shader, Camera& camera)
{   
    for (int i = 0; i < button_mesh.size(); i++)
    {
        button_mesh[i].updateVertices(Vertices);
        button_mesh[i].Draw(shader, camera);
    }
}

bool Button::interact(GLFWwindow* window, int width, int height)
{
    double mx, my;
    glfwGetCursorPos(window, &mx, &my);
    
    //Normalizes mouse x and y coords for GLFW (-1 to 1 values only)
    mx = (mx * 2 - width) / width;
    my = -((my * 2 - height) / height);

    //IF MOUSE IF HOVERING WITHIN BUTTON
    if (mx < Vertices[0].position.x && mx > Vertices[1].position.x 
        && my < Vertices[1].position.y && my > Vertices[2].position.y)
        {
        //Change button color from grey to pale yellow
        for (int i = 0; i < Vertices.size() - 2 ; i++)
        {
            Vertices[i].color.x += 0.2f;
            Vertices[i].color.y += 0.2f;
        }
        //Captures mouse state for left-button click (1 if clicked, 0 if not)
        int mouse_state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

        //If left mouse button is pressed
        if (mouse_state == GLFW_PRESS && !pressed)
        {
            //std::cout << "---RESET LEVEL---\n";
            pressed = true;
            return true;
        }
        //If left mouse button is released
        else if (mouse_state == GLFW_RELEASE)
        {
            pressed = false;
        }
    }
    else
    {
        for (int i = 0; i < Vertices.size() ; i++)
        {   
            Vertices[i].color.x = 0.2f;
            Vertices[i].color.y = 0.2f;
            Vertices[i].color.y = 0.2f;
        }
        pressed = false;
    }
    return false;
}