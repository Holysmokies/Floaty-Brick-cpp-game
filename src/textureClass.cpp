#include <glad/glad.h>
#include <stb/stb_image.h>

#include "shaders/shaderClass.h"
#include "textures/textureClass.h"

Texture::Texture(const char* image,  const char* texType, GLuint slot)
{   
    //NULL USED FOR LIGHTS
    if (std::string(image) == "NULL")
    {
        type = texType;
    }
    else
    {
        type = texType;
        int widthImg, heightImg, colorChannels;
        //flips img/texture on y axis becuase OpenGL reads from bottomleft to topright while STB reads topleft to bottom right  
        stbi_set_flip_vertically_on_load(true);

        unsigned char* bytes = stbi_load(image, &widthImg, &heightImg, &colorChannels, 0);

        //Generate texture, ACTIVEates it and BINDS it for use/manipulation
        glGenTextures(1, &ID);
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, ID);
        unit = slot;

        //(GL_NEAREST great for pixel art, GL_LINEAR smoothes, but makes textures blurry)
        //when texture is being rendered smaller  vvv, 			render style vvv 
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
        //when texture is being rendered larger   vvv
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        //Tells OpenGL how to tile/fit texutre (x, y, z) = (s, t, r)
        //GL_REPEAT, GL_MIRRORED REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

        //EXAMPLE GL_CLAMP_TO_BORDER
        //float flatcolor[] = {0.2f, 0.5f, 0.5f, 1.0f}
        //glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S/T/R, GL_CLAMP TO BORDER, flatcolor)

        //Generate Image (PNG uses RGBA - JPEG uses RGB)
        if (colorChannels == 4) //RGBA
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RGBA, GL_UNSIGNED_BYTE, bytes);
        }
        else if (colorChannels == 3) //RGB
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RGB,  GL_UNSIGNED_BYTE, bytes);
        }
        else if (colorChannels == 1) //R - used for specular maps
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, widthImg, heightImg, 0, GL_RED, GL_UNSIGNED_BYTE, bytes);
        }
        else throw std::invalid_argument("---Texture type recognition failure.---");


        //MIPMAPS reduce aliasing and help retain quality when scaling textures
        glGenerateMipmap(GL_TEXTURE_2D);

        //frees up memory used by image
        stbi_image_free(bytes);
        //unbinds texture
        glBindTexture(GL_TEXTURE_2D, 0);
    }
};

void Texture::texUnit(Shader& shader, const char* uniform, GLuint unit)
{   
    //allocates location in memory for universal to occupy
    GLuint texUniform = glGetUniformLocation(shader.ID, uniform);
    //SHADER MUST BE ACTIVATED BEFORE UNIFORM USE - glUseProgram(shaderProgram)
    shader.Activate();
    glUniform1i(texUniform, unit);
}

void Texture::Bind()
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, ID);
}

void Texture::Unbind()
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::Delete()
{
    glDeleteTextures(1, &ID);
}