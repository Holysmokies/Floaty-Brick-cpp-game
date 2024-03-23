#include <cstdlib>
#include <ctime>

#include "worldObjects/obstacles/obstacles.h"


Pipe new_pipe(std::vector <Vertex>& vertices, std::vector <GLuint>& indices, std::vector <Texture>& textures, float x_mod, float y_mod);

std::vector <Vertex> focus_pipe(std::vector <Pipe> pipe_list, std::vector <Vertex> player_verts, std::vector <Vertex> current_focus);

std::vector <Pipe> delete_pipes(std::vector <Pipe> pipe_list);