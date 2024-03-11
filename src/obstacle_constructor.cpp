#include "worldObjects/obstacles/obstacle_constructor.h"

Pipe new_pipe(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures, float x_mod, float y_mod)
{
    std::vector <Vertex> new_verts = vertices;
    for (int i = 0; i < vertices.size(); i++)
    {
        new_verts[i].position.x += x_mod;
        new_verts[i].position.y += y_mod;
    }
    Pipe new_pipe(new_verts, indices, textures);
    return new_pipe;
}
