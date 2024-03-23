#ifndef SHADER_CLASS_H
#define SHADER_CLASS_H

#include <glad/glad.h>
#include <string>
#include <fstream>
//string streams = sstream
#include <sstream>
#include <iostream>
//c error handling = cerrno
#include <cerrno>

std::string get_file_contents(const char* filename);

class Shader
{
    public:
        //Reference ID for shader program
        GLuint ID;
            //object constructor using two shaders - vert and frag
            Shader(const char* vertexFile, const char* fragmentFile);

            void Activate();
            void Delete();
    private:
        void compileErrors(unsigned int shader, const char* tpye);
};

#endif
