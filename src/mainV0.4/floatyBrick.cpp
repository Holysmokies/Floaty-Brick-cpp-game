#include <iostream>
#include <cmath>
#include <filesystem>
#include <windows.h>

#include "mesh/2d_mesh_class.h"
#include "backgroundColor/backgroundColor.h"
#include "player/player2d.h"
#include "worldObjects/obstacles/obstacle_constructor.h"
#include "buttons/button.h"


#define GLT_IMPLEMENTATION
#include "gltext/gltext.h"

namespace fs = std::filesystem;
////----------------------------------------------------------------------------------------//
/*----------TO-DO LIST----------
*URGENT*
*  1. Check/Fix memory leak...
        1a. FIXED --- MASSIVE - PLAYER INPUTS() --- ([same as 1c] changed mesh class to update VBO vertices instead of creating new Mesh)
        1b. FIXED --- SMALL - PLAYER DRAW()
        1c. FIXED --- MASSIVE - OBJECT/PIPE DRAW() 

*  2. Game Crashes/Nonresponsive
        2a. FIXED --- Main loop pipe erase (massive memory leak and unresponsive, endless new pipe generation?)  
                  --- (infinite score point increase meant new pipe.position.x threshhold was always met)
--------------------------------
*BUG FIXES*

21 April 2024
---Fixed issue where fragment shader was either showing no textures for anything, or no colors (black) for objects without a texture


--------------------------------
*NORMAL*
        (DONE)
*  1. Adjust player gravity, remove ability to continuously jump (only rejump after speed is sufficiently negative)
        (DONE)
*  2a. Add functionality to obscales file/class to create its own objects and randomize heights 
        given a close enough threshold to previous pipe's height.
        (DONE)
*  2b. Add Pipe Mirroring to top of screen.
        (DONE)
*  3a. Add horizontal (and vertical?) collision checking to player and obstacles.
        (DONE)
*  3b. Fix collision code (HORIZONTAL/X CHECKING FIRST)
            --- Required slight rewrite of player function: collision_detection(), since memory for variables was being lost.
            --- Also had to update the 2d_mesh_class function: updateVertices() as object/mesh vertices were not properly updating.
        (DONE)
*  3c. Fix double collisions (right side then left side)
            --- Moved player_colliding variable to the focus checking so that it is only reset after a new focus is selected
            --- to avoid multiple collisions on the same obstacle through multiple vertices
        (DONE)
*  4. Implement focus updating after obstacle goes left (negative x values) of player to properly fix collision detection.
        (DONE)
*  5a. Add score text to top right screen corner (glText).
        (DONE)
*  5b. Create fail state (text on screen).
            ---Fail state for collision.
            ---Fail state for falling off screen.
        (DONE)
* 5c. Implement button/option to restart) (GLFW, dear imgui, NanoGUI?).
            ---Implemented a simple GLFW shape object with mouseover checking.
            ---Added left-click check and changed button interact() to return bool value. 
            ---Reset game and player variables, and pipe lists upon clicking button.
            ---Added GLText "Retry?".
        (DONE)
*  6. Add textures.
        (DONE)
*  7. Create difficulty increase over time.
            ---Increased the rate at which new pipes spawn based on score.
            ?--Implement pipe speed increase after max spawn rate achieved?
        (DONE)
*  8. Fix score count updating (how to update when new pipe is focused?)
            --- Created old focus variable to cross check the index in list with new focus

*  9. Create start screen/GUI.

*  10. Implement game music/sounds.

*  11a. Create player level system/talent tree? (is this worth?).

*  11b. Create power-up system (immunity, skip/rocket boost, time-slow?).

*/
//----------------------------------------------------------------------------------------//
//establish system height and width for windowed fullscreen (?) mode
int windowWidth = GetSystemMetrics(SM_CXSCREEN);
int windowHeight = GetSystemMetrics(SM_CYSCREEN);
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
    Vertex{glm::vec3(player_x_right,  player_height/2, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)}, //top right
    Vertex{glm::vec3(player_x_left,   player_height/2, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)}, //top left
    Vertex{glm::vec3(player_x_right, -player_height/2, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)}, //bottom right
    Vertex{glm::vec3(player_x_left,  -player_height/2, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)}, //bottom left
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
    Vertex{glm::vec3(1.1f, -0.1f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)}, //top right
    Vertex{glm::vec3(1.0f, -0.1f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)}, //top left
    Vertex{glm::vec3(1.1f, -1.5f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)}, //bottom right
    Vertex{glm::vec3(1.0f, -1.5f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)}, //bottom left
};

GLuint pipeIndices[] = 
{
    0,1,2,
    1,2,3
}; 
//----------------------------------------------------------------------------------------//
Vertex retry_button_verts[] = 
{
    Vertex{glm::vec3(0.15f, -0.3f, 0.0f), glm::vec3(0.3f, 0.3f, 0.3f), glm::vec2(1.0f, 1.0f)}, //top right
    Vertex{glm::vec3(-0.15f, -0.3f, 0.0f), glm::vec3(0.3f, 0.3f, 0.3f), glm::vec2(0.0f, 1.0f)}, //top left
    Vertex{glm::vec3(0.15f, -0.5f, 0.0f), glm::vec3(0.3f, 0.3f, 0.3f), glm::vec2(1.0f, 0.0f)}, //bottom right
    Vertex{glm::vec3(-0.15f, -0.5f, 0.0f), glm::vec3(0.3f, 0.3f, 0.3f), glm::vec2(0.0f, 0.0f)}, //bottom left
};

GLuint retry_button_indcs[] = 
{
    0,1,2,
    1,2,3
};

//----------------------------------------------------------------------------------------//
void gameOverText(int width, int height)
{
    GLTtext *gameOver_text = gltCreateText();
    gltSetText(gameOver_text, "Game Over.");

    gltBeginDraw();
    gltColor(1.0f, 0.0f, 0.0f, 1.0f);
    gltDrawText2DAligned(gameOver_text, width/2, height/2, 8.0f, GLT_CENTER, GLT_CENTER);
    gltEndDraw();
    gltDeleteText(gameOver_text);
}

void scoreText(int width, int height, int score)
{
    GLTtext *score_text = gltCreateText();
    GLTtext *score_text_count = gltCreateText();
    gltSetText(score_text, "Score: ");

    char str[4];
    sprintf(str, "%d", score);
    gltSetText(score_text_count, str);

    gltBeginDraw();
    gltColor(1.0f, 1.0f, 1.0f, 1.0f);
    gltDrawText2DAligned(score_text, width - ((score_text->_textLength + 1) * sizeof(GLTtext)), 
                            (score_text->_textLength * sizeof(GLTtext)) / 4, 4.0f, GLT_LEFT, GLT_BOTTOM);
    gltDrawText2DAligned(score_text_count, width - (2.9 * sizeof(GLTtext)), 
                            (score_text->_textLength  * sizeof(GLTtext)) / 4, 4.0f, GLT_LEFT, GLT_BOTTOM);
    gltEndDraw();
    gltDeleteText(score_text);
    gltDeleteText(score_text_count);
}

void retryText(int width, int height)
{
    GLTtext *retry_text = gltCreateText();
    gltSetText(retry_text, "Retry?");

    gltBeginDraw();
    gltColor(1.0f, 1.0f, 1.0f, 1.0f);
    gltDrawText2DAligned(retry_text, width/2, height/2 + 190, 4.0f, GLT_CENTER, GLT_CENTER);
    gltEndDraw();
    gltDeleteText(retry_text);
}

int main()
{   
    //std::cout << windowWidth << "x" << windowHeight << std::endl;
    //std::cout << widthAdjustment << std::endl;
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
    window = glfwCreateWindow(windowWidth, windowHeight, "Floaty Block v0.3", NULL, NULL);
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

    //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //-------------------------------------------------------------------------------------------//
    //Define and build object shaders
    std::string vert_shader_path2d = current_dir + "/include/shaders/default2d.vert";
    std::string frag_shader_path2d = current_dir + "/include/shaders/default2d.frag";
    const char* vert_path2d = (vert_shader_path2d).c_str();
    const char* frag_path2d = (frag_shader_path2d).c_str();
    Shader shaderProgram2d(vert_path2d, frag_path2d);
    //------------------------------------------------------------------------------------//
    //Initialize camera
    glEnable(GL_DEPTH_TEST);
    Camera camera(windowWidth, windowHeight, glm::vec3(0.0f, 0.5f, 4.0f));
    //-------------------------------------------------------------------------------------------//
    //Construct player object
    Texture playerTextures[] 
    {
        Texture((current_dir + "/include/textures/red_brick.png").c_str(), "diffuse", 0),
    };

    std::vector <Vertex> playerVerts(playerVertices, playerVertices + sizeof(playerVertices) / sizeof(Vertex));
    std::vector <GLuint> playerIndcs(playerIndices, playerIndices + sizeof(playerIndices) / sizeof(GLuint));
    std::vector <Texture> playerText(playerTextures, playerTextures + sizeof(playerTextures) / sizeof(Texture));
    
    Player Player(playerVerts, playerIndcs, playerText);
    //-------------------------------------------------------------------------------------------//
    //Construct obstacles/world objects
    Texture pipeTextures[] 
    {
        Texture((current_dir + "/include/textures/bones_even_thinner.png").c_str(), "diffuse", 0),
    };

    std::vector <Vertex> pipeVerts(pipeVertices, pipeVertices + sizeof(pipeVertices) / sizeof(Vertex));
    std::vector <GLuint> pipeIndcs(pipeIndices,  pipeIndices  + sizeof(pipeIndices) / sizeof(GLuint));
    std::vector <Texture> pipeText(pipeTextures, pipeTextures + sizeof(pipeTextures) / sizeof(Texture));
    //----------------------------------------------------------------------------------------------------------//
    Texture buttonTexts[]
    {
        Texture("NULL", "NULL", 0),
    };

    std::vector <Vertex> retryVerts(retry_button_verts, retry_button_verts + sizeof(retry_button_verts) / sizeof(Vertex));
    std::vector <GLuint> retryIndcs(retry_button_indcs,  retry_button_indcs  + sizeof(retry_button_indcs) / sizeof(GLuint));
    std::vector <Texture> retryTexture(buttonTexts, buttonTexts + sizeof(buttonTexts) / sizeof(Texture));

    Button retry_button(retryVerts, retryIndcs, retryTexture);
    //----------------------------------------------------------------------------------------------------------//
    //GAME VARIABLES

    //background color variables
    glm::vec3 background_color = glm::vec3(1.0f);
    double prevTime = glfwGetTime();
    double curTime = 0;
    float frameRate = 1/60;
    float color_factor = 0.001f; 
    bool red_dim = true;
    bool green_dim = true;
    bool blue_dim = true;

    //Seed random number generator
    srand (static_cast <unsigned> (time(0)));

    //x_amp amplifies difficulty/frequency of pipes as score increass
    float score = 0;
    float x_amp = 0.0f;

    //pipe variables
    float min_height = pipeVertices[0].position.y - 0.7f;
    float max_height = min_height + 1.0f;
    float x_modifier = 0.75f;
    //Draw upper pipe Ypos as ---> x (gap between lower) + y (height of pillar offset) 
    float upper_pipe_y_modifier = 0.8f + 1.5f;

    //Pipe lists
    std::vector <Pipe> pipes_lower;
    std::vector <Pipe> pipes_upper;
    pipes_lower.push_back(new_pipe(pipeVerts, pipeIndcs, pipeText, 0.0f, 0.0f));
    pipes_upper.push_back(new_pipe(pipeVerts, pipeIndcs, pipeText, 0.0f, upper_pipe_y_modifier));

    //focus pipes (upper and lower)
    std::vector <Vertex> focus_upper = pipes_upper[0].Vertices;
    std::vector <Vertex> focus_lower = pipes_lower[0].Vertices;
    int previous_focus_index = 0;
    int current_focus_index;

    //death variables
    bool collision = false;
    bool death_bounce = false;

    //-------------------------------------------------------------------------------------------//
    //CREATE GLTEXT instance for use in game (game over, score count, retry, etc)
    gltInit();
    //-------------------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------//
    //-------------------------------------------------------------------------------------------------------------//
    //WHILE WINDOW IS OPEN
    while (!glfwWindowShouldClose(window))
    {  
        //-------------------------------------------------------------------------------------------//
        //Initial window management
        if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {glfwSetWindowShouldClose(window, GLFW_TRUE);}
			
        glfwGetFramebufferSize(window, &windowWidth, &windowHeight);
        glViewport(0, 0, windowWidth, windowHeight);
        
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
        shaderProgram2d.Activate();
        camera.updateMatrix(45.0f, 0.1f, 100.0f);
        //Update player position based on inputs and draw player
        Player.Inputs(window);
        Player.Draw(shaderProgram2d, camera);
        //-------------------------------------------------------------------------------------------//
        //FAIL STATE (player collision)
        if (Player.player_colliding || Player.player_off_screen) 
        {
            gameOverText(windowWidth, windowHeight);
            scoreText(windowWidth, windowHeight, score);
            retryText(windowWidth, windowHeight);
            
            retry_button.Draw(shaderProgram2d, camera);

            if (!death_bounce)
            {   
                Player.FallTime = 0.0f;
                Player.Speed += 0.0002f;
                death_bounce = true;
            }
            
            for (int i = 0; i < pipes_lower.size(); i++)
            {   
                pipes_lower[i].scroll_speed = 0;
                pipes_upper[i].scroll_speed = 0;
                pipes_lower[i].Pipe::Draw(shaderProgram2d, camera);
                pipes_upper[i].Pipe::Draw(shaderProgram2d, camera);
            }

            //IF reset button is pressed
            if (retry_button.interact(window, windowWidth, windowHeight))
            {
                std::cout << "---RESET GAME---\n";

                //Delete pipe lists and reset focus
                pipes_lower.clear();
                pipes_upper.clear();
                pipes_lower.push_back(new_pipe(pipeVerts, pipeIndcs, pipeText, 0.0f, 0.0f));
                pipes_upper.push_back(new_pipe(pipeVerts, pipeIndcs, pipeText, 0.0f, upper_pipe_y_modifier));

                focus_upper = pipes_upper[0].Vertices;
                focus_lower = pipes_lower[0].Vertices;
                previous_focus_index = 0;
                current_focus_index = 0;

                //Reset player variables
                death_bounce = false;
                Player.player_colliding = false;
                Player.reset_position(widthAdjustment);
                Player.reset_score();
                Player.FallTime = 0;
                Player.player_off_screen = false;

                //Reset game variables
                x_amp = 0.0f;
                score = 0;
            }
        }
        //-------------------------------------------------------------------------------------------//
        //NORMAL STATE (no collision)
        else
        {   
            scoreText(windowWidth, windowHeight, score);
            //-------------------------------------------------------------------------------------------//
            //FOCUS PIPES

            //Sets pipe as FOCUS to check for collision detection 
            focus_upper = focus_pipe(pipes_upper, Player.Vertices, focus_upper);
            focus_lower = focus_pipe(pipes_lower, Player.Vertices, focus_lower);
            
            //Identifies index of focus pipe within pipe list
            for (int i = 0; i < pipes_lower.size(); i++)
            {
                if (pipes_lower[i].Vertices[0].position.x == focus_lower[0].position.x)
                {
                    current_focus_index = i;
                }
            }   
            //Checks if current focus pipe is the same as previous focus pipe (index)
            if (current_focus_index != previous_focus_index)
            {
                Player.score += 1;
                previous_focus_index = current_focus_index;
                //reset collision?
            }
            
            //-------------------------------------------------------------------------------------------//
            //COLLISION DETECTION

            //checks if any player vertex is colliding with object
            for (int i = 0; i < Player.Vertices.size(); i++)
            {  
                if ((Player.collision_detection(Player.Vertices[i].position.x, Player.Vertices[i].position.y, focus_upper))
                    || Player.collision_detection(Player.Vertices[i].position.x, Player.Vertices[i].position.y, focus_lower))
                {   
                    collision = true;
                    break;
                }
            }

            //if player collides during this loop and is not already colliding (in previous loop)
            if (collision && !Player.player_colliding) 
            {
                Player.player_colliding = true;
            }
            //resets collision for next loop, but keeps player_colliding as it was set this loop (for history checking in next loop)
            collision = false;
            //--------------------------------------------------------------------------------------------//
            //DELETE PIPES when they go off screen to the LEFT
            if (!pipes_lower[0].on_screen && pipes_lower[0].Vertices[0].position.x <= -1.2f)
            {
                pipes_lower.erase(pipes_lower.begin());
                pipes_upper.erase(pipes_upper.begin());
                Player.score -= 1;
            }
            //-------------------------------------------------------------------------------------------//
            //DRAW AND CREATE PIPES
            for (int i = 0; i < pipes_lower.size(); i++)
            {   
                pipes_lower[i].Pipe::Draw(shaderProgram2d, camera);
                pipes_upper[i].Pipe::Draw(shaderProgram2d, camera);

                //CREATE NEW PIPES ON SET INTERVALS
                if (abs(1.0f - pipes_lower[i].Vertices[0].position.x) <= 0.3f + x_amp && pipes_lower.size() < i+2)
                {   
                    //std::cout << "---NEW PIPE---\n";
                    float y_modifier = min_height + static_cast <float> (rand()) / ( static_cast <float> (RAND_MAX / (max_height - min_height)));
                    pipes_lower.push_back(new_pipe(pipeVerts, pipeIndcs, pipeText, x_modifier, y_modifier));
                    pipes_upper.push_back(new_pipe(pipeVerts, pipeIndcs, pipeText, x_modifier, y_modifier + upper_pipe_y_modifier));
                }
            }
            //-------------------------------------------------------------------------------------------//
            //SCORE and DIFFICULTY
            if (score != Player.score) 
            {
                //std::cout << "PLAYER SCORE: " << Player.score << std::endl;
                score = Player.score;
                if (x_amp < 0.19f)
                {
                    x_amp += 0.02f;
                    //std::cout << "DIFFICULTY: " << x_amp << std::endl;
                }
            }
        }
        //-------------------------------------------------------------------------------------------//

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    shaderProgram2d.Delete();
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
