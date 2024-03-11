#include "shaders/shaderClass.h"
#include <vector>

std::string get_file_contents(const char* filename)
{   
    //naming file "in"
    std::ifstream in(filename, std::ios::binary);
    //if file exists
    if (in)
    {
        std::cout << filename << " - exists" << std::endl;
        std::string contents;
        //SEEKG is function of the iostream/fstream lib that allows us to
        //seek an arbitrary position in a file, in this case it is the END of file.
        in.seekg(0, std::ios::end);

        //TELLG returns an integer based on where the pointer currently is
        //so after using in.seekg() above it should be on the last char of file.
        //We then use this to resize our string 'contents' to the correct size.
        contents.resize(in.tellg());

        //Resets the position of the pointer to the beginning of the file for reading.
        in.seekg(0, std::ios::beg);

        //Reads file and writes starting with contents[0] until length has been reached.
        in.read(&contents[0], contents.size());
        in.close();
        //std::cout << "Returning:\n" << contents << std::endl;
        return contents;
    }
    std::cout << "ERROR WITH -- " << filename << "\n"; 
    throw(errno);
}

Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
    std::string vertexCode = get_file_contents(vertexFile);
    std::string fragmentCode = get_file_contents(fragmentFile);

    //converts strings into character arrays
    const char* vertexSource = vertexCode.c_str();
    const char* fragmentSource = fragmentCode.c_str();

    //VERTEX SHADER CREATION AND COMPILE
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //Attach vertex shader source code to vertex object
    glShaderSource(vertexShader, 1, &vertexSource, NULL);
    //Compile vertex shader into machine code
    glCompileShader(vertexShader);
    compileErrors(vertexShader, "VERTEX");

    //FRAGMENT SHADER CREATION AND COMPILE
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
    glCompileShader(fragmentShader);
    compileErrors(fragmentShader, "FRAGMENT");

    //Create Shader program object and get reference
    ID = glCreateProgram();
    //Attach both vertex and fragment shaders to the shader program
    glAttachShader(ID, vertexShader);
    glAttachShader(ID, fragmentShader);
    //Link all shaders together in the program
    glLinkProgram(ID);
    compileErrors(ID, "PROGRAM");

    //clean up memory and delete the now useless shader objects 
    //(because they exist in the memory of the shader program now)
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Shader::Activate()
{
    glUseProgram(ID);
}

void Shader::Delete()
{
    glDeleteProgram(ID);
}

//CHECKS THAT EACH SHADER (FRAG/VERT) COMPILES AND LINKS TO SHADER PROGRAM
void Shader::compileErrors(unsigned int shader, const char* type)
{
    GLint hasCompiled;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE)
        {
            std::cout <<"--- " << type << " SHADER NOT COMPILED ---" << std::endl;
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            //OUTPUTS LOG IF ISSUES ARISE
            for (auto element : infoLog) {
                std::cout << element;
            }
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &hasCompiled);
        if (hasCompiled == GL_FALSE)
        {
            std::cout << "--- " << type <<" NOT LINKED ---" << std::endl;
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            for (auto element : infoLog) {
                std::cout << element;
            }
        }
    }
}