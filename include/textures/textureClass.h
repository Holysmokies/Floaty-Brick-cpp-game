#ifndef TEXTURE_CLASS_H
#define TEXTURE_CLASS_H

#include <glad/glad.h>
#include <stb/stb_image.h>

#include "shaders/shaderClass.h"

class Texture
{
    public:
        GLuint ID;
        const char* type;
        GLuint unit;

        Texture(const char* image,  const char* texType, GLuint slot);

        void texUnit(Shader& shader, const char* tex_uniform, GLuint unit); //const char* tex_exists);
        void Bind();
        void Unbind();
        void Delete();
};

#endif
