#include "shaderBuffers/2dVBO.h"

VBO::VBO()
{
    glGenBuffers(1, &ID);
};

void VBO::Construct(std::vector<Vertex>& vertices)
{
    glBindBuffer(GL_ARRAY_BUFFER, ID);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
}
void VBO::Bind()
{
    glBindBuffer(GL_ARRAY_BUFFER, ID);
}

void VBO::Unbind()
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VBO::Delete()
{
    glDeleteBuffers(1, &ID);
}