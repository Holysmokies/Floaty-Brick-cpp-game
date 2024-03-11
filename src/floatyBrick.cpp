#include <iostream>
#include <cmath>
#include <filesystem>
#include <windows.h>

#include "mesh/2d_mesh_class.h"
#include "backgroundColor/backgroundColor.h"
#include "player/player2d.h"
#include "worldObjects/obstacles/obstacle_constructor.h"

namespace fs = std::filesystem;
////----------------------------------------------------------------------------------------//
/*-----TO-DO LIST-----
*URGENT*
*  1. Check/Fix memory leak...
        1a. MASSIVE - PLAYER INPUTS() --- FIXED ([same as 1c] changed mesh class to update VBO vertices instead of creating new Mesh)
        1b. SMALL - PLAYER DRAW() --- FIXED
        1c. MASSIVE - OBJECT/PIPE DRAW() ---FIXED 

*  2. Game Crashes/Nonresponsive
        2a. Main loop pipe erase (massive memory leak and unresponsive, endless new pipe generation?) --- FIXED 
                (infinite score point increase meant new pipe x position threshhold was always met)

----------------------
*NORMAL*
        (DONE)
*  1. Adjust player gravity, remove ability to continuously jump (only rejump after speed is sufficiently negative)
        (DONE)
*  2. Add functionality to obscales file/class to create its own objects and randomize heights 
        given a close enough threshold to previous pipe's height.

*  2b. Add Pipe Mirroring to top of screen.

*  3. Add horizontal (and vertical?) collision checking to player and obstacles.

*  4. Create fail state (do with #3).

*  5. Add textures.

*  6. Create start screen.

*  7. Create point system/difficulty increase.

*  8. Create level system.
*/
//----------------------------------------------------------------------------------------//
//establish system height and width for windowed fullscreen mode
const int windowWidth = GetSystemMetrics(SM_CXSCREEN);
const int windowHeight = GetSystemMetrics(SM_CYSCREEN);
const float widthAdjustment = (float)windowWidth / (float)windowHeight;

//windows constant
std::string parent_dir = fs::current_path().fs::path::parent_path().string();
//edit depending on file save location
std::string working_dir = "/floaty_brick";
//for use in code
std::string current_dir = parent_dir + working_dir;

//----------------------------------------------------------------------------------------//
//PLAYER VERTICES AND INDICES
//VERTEX struct defined in VBO.h
float player_height = 0.2f;
float player_x_left = -0.6f;
float player_x_right = player_x_left + (player_height / widthAdjustment);

Vertex playerVertices[] = 
{
    Vertex{glm::vec3(player_x_right, player_height/2, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)},
    Vertex{glm::vec3(player_x_left, player_height/2, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)},
    Vertex{glm::vec3(player_x_right, -player_height/2, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)},
    Vertex{glm::vec3(player_x_left, -player_height/2, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)},
};

GLuint playerIndices[] = 
{
    0,1,2,
    1,2,3
}; 
//----------------------------------------------------------------------------------------//
//OBSTACLE VERTICES AND INDICES
Vertex pipeVertices[] = 
{
    Vertex{glm::vec3(1.1f,  0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 1.0f)}, //top right
    Vertex{glm::vec3(1.0f,  0.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 1.0f)}, //top left
    Vertex{glm::vec3(1.1f, -2.5f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(1.0f, 0.0f)}, //bottom right
    Vertex{glm::vec3(1.0f, -2.5f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec2(0.0f, 0.0f)}, //bottom left
};

GLuint pipeIndices[] = 
{
    0,1,2,
    1,2,3
}; 
//----------------------------------------------------------------------------------------//
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

int main()
{   
    std::cout << windowWidth << "x" << windowHeight << std::endl;
    std::cout << widthAdjustment << std::endl;
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW" << std::endl;
        return -1;
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
    //TELLS GLFW WHAT VERSION IT IS - 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //PACKAGE FUNCTIONS - CORE 
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //-------------------------------------------------------------------------------------------//
    //CREATES WINDOW
    GLFWwindow* window;
    window = glfwCreateWindow(windowWidth, windowHeight, "Floaty Block v0.1", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to open GLFW window" << std::endl;
        return -1;
    }
    //TELLS GLFW TO USE WINDOW and initializes GLAD
    glfwMakeContextCurrent(window);
    //gladLoadGL();
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    //Specify viewport (size of window) for OpenGL
    glViewport(0, 0, windowWidth, windowHeight);

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //-------------------------------------------------------------------------------------------//
    //Define and build object shaders
    std::string vert_shader_path2d = current_dir + "/include/shaders/default2d.vert";
    std::string frag_shader_path2d = current_dir + "/include/shaders/default2d.frag";
    const char* vert_path2d = (vert_shader_path2d).c_str();
    const char* frag_path2d = (frag_shader_path2d).c_str();
    Shader shaderProgram2d(vert_path2d, frag_path2d);
    

    std::string vert_shader_path3d = current_dir + "/include/shaders/default3d.vert";
    std::string frag_shader_path3d = current_dir + "/include/shaders/default3d.frag";
    const char* vert_path3d = (vert_shader_path3d).c_str();
    const char* frag_path3d = (frag_shader_path3d).c_str();
    Shader shaderProgram3d(vert_path3d, frag_path3d);
    //-------------------------------------------------------------------------------------------//
    //Initialize camera
    glEnable(GL_DEPTH_TEST);
    Camera camera(windowWidth, windowHeight, glm::vec3(0.0f, 0.5f, 4.0f));
    //-------------------------------------------------------------------------------------------//
    //Construct player object
    Texture playerTextures[] {Texture("NULL", "NULL", 1)};

    std::vector <Vertex> playerVerts(playerVertices, playerVertices + sizeof(playerVertices) / sizeof(Vertex));
    std::vector <GLuint> playerIndcs(playerIndices, playerIndices + sizeof(playerIndices) / sizeof(GLuint));
    std::vector <Texture> playerText(playerTextures, playerTextures + sizeof(playerTextures) / sizeof(Texture));
    
    Player player(playerVerts, playerIndcs, playerText);
    //-------------------------------------------------------------------------------------------//
    //Construct obstacles/world objects
    Texture pipeTextures[] {Texture("NULL", "NULL", 1)};

    std::vector <Vertex> pipeVerts(pipeVertices, pipeVertices + sizeof(pipeVertices) / sizeof(Vertex));
    std::vector <GLuint> pipeIndcs(pipeIndices,  pipeIndices  + sizeof(pipeIndices) / sizeof(GLuint));
    std::vector <Texture> pipeText(pipeTextures, pipeTextures + sizeof(pipeTextures) / sizeof(Texture));
    
    //Pipe pipe1(pipeVerts, pipeIndcs, pipeText);
    //-------------------------------------------------------------------------------------------//
    //Seed random number generator
    srand (static_cast <unsigned> (time(0)));
    
    double prevTime = glfwGetTime();
    double curTime = 0;
    float frameRate = 1/60;
    float color_factor = 0.001f; 
    bool red_dim = true;
    bool green_dim = true;
    bool blue_dim = true;

    //x_amp amplifies difficulty over time
    float x_amp = 0.0f;
    int score_count = 0;
    bool deleting = false;

    float min = pipeVertices[0].position.y - 0.2f;
    float max = min + 0.6f;
    float x_modifier = 0.75f;

    glm::vec3 background_color = glm::vec3(1.0f);

    std::vector <Pipe> pipes;
    pipes.push_back(Pipe(pipeVerts, pipeIndcs, pipeText));


    //WHILE WINDOW IS OPEN
    while (!glfwWindowShouldClose(window))
    {  
        glClearColor(background_color.x, background_color.y, background_color.z, 1.0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //-------------------------------------------------------------------------------------------//
        //Sets dynamic background color
        curTime = glfwGetTime();
        if (curTime - prevTime >= frameRate)
        {   
            red_dim = bool_check(background_color.x, red_dim);
            green_dim = bool_check(background_color.y, green_dim);
            blue_dim = bool_check(background_color.z, blue_dim);

            background_color.x = red_change(background_color.x, red_dim);
            background_color.y = green_change(background_color.y, green_dim);
            background_color.z = blue_change(background_color.z, blue_dim);

            prevTime = curTime;
        }
        //-------------------------------------------------------------------------------------------//
        //Main Game Loop
        camera.updateMatrix(45.0f, 0.1f, 100.0f);
        
        player.Inputs(window);
        player.Draw(shaderProgram2d, camera);

        //Delete pipe when goes off screen to the left
        if (!pipes[0].on_screen && pipes[0].Vertices[0].position.x <= -1.2f)
        {
            //std::cout << "---DELETE PIPE---\n";
            pipes.erase(pipes.begin());
        }

        for (int i = 0; i < pipes.size(); i++)
        {   
            pipes[i].Pipe::Draw(shaderProgram2d, camera);

            //Create new pipe on set intervals
            if (abs(1.0f - pipes[i].Vertices[0].position.x) <= 0.3f + x_amp && pipes.size() < i+2)
            {   
                float y_modifier = min + static_cast <float> (rand()) /( static_cast <float> (RAND_MAX/(max-min)));
                pipes.push_back(new_pipe(pipeVerts, pipeIndcs, pipeText, x_modifier - x_amp, y_modifier));
                //std::cout << "---X-THRESHOLD REACHED: " << abs(1.0f - pipes[i].Vertices[0].position.x) << "\n---DRAW NEW PIPE---" << i+1 << "x" << pipes.size() << std::endl;
            }
        }

        x_amp += 0.002*score_count;

        glfwSwapBuffers(window);
        glfwPollEvents(); 
    }
    shaderProgram2d.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}