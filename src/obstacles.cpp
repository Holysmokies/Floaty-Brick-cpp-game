#include "worldObjects/obstacles/obstacles.h"

Pipe::Pipe(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures)
{
    Vertices = vertices; 
    Indices = indices;
    Textures = textures;
    meshes.push_back(Mesh(Vertices, Indices, Textures));
}

void Pipe::Draw(Shader& shader, Camera& camera)
{
    check_on_screen();
    updatePosition();
    for (int i = 0; i < meshes.size(); i++)
    {   
        //DRAW IF ON SCREEN (between 1.2 and -1.2 x-coords)
        if (on_screen)
        {
            meshes[i].updateVertices(Vertices);
            meshes[i].Draw(shader, camera);
        } 
        //DELETE IF OFF LEFT SIDE OF SCREEN 
        if (!on_screen && Vertices[0].position.x < -1.2f)
        {

        }
    }
}

void Pipe::updatePosition()
{
    for (int i = 0; i < Pipe::Vertices.size(); i++)
    {
        Vertices[i].position.x += scroll_speed;
    }
}

void Pipe::check_on_screen()
{
    for (int i = 0; i < Pipe::Vertices.size(); i++)
    {
        if (Pipe::Vertices[i].position.x < -1.2f || Pipe::Vertices[i].position.x > 1.2f)
        {
            on_screen = false;
        }
        else
        {
            on_screen = true;
        }
    }
}