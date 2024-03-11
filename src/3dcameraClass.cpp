#include "camera/3dcameraClass.h"



Camera::Camera(int width, int height, glm::vec3 position)
{
    Camera::width = width;
    Camera::height = height;
    Position = position;
}

void Camera::updateMatrix(float FOVdegree, float nearPlane, float farPlane)
{
    glm::mat4 view = glm::mat4(1.0f);
    glm::mat4 projection = glm::mat4(1.0f);

    view = glm::lookAt(Position, Position + Orientation, Up);
    //tells OpenGL what to display and what to clip                                 close  , far clip 
    //                                                                            vvvvvvvvv, vvvvvvvv
    projection = glm::perspective(glm::radians(FOVdegree), float(width / height), nearPlane, farPlane);

    //creates uniform matrix to use in vertex shader
    cameraMatrix = projection * view;
}

void Camera::Matrix(Shader& shader, const char* uniform)
{
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(cameraMatrix));
}

void Camera::Inputs(GLFWwindow* window)
{       
    // "Orientation" is a glm vector object which specifies the Z plane to manipulate where
    //      Orientation is (0.0f, 0.0f, -1.0f), therefore multiplying it by camera speed (key W)
    //      results in objects being drawn subtracted (closer to screen) from previous position.

    // "Up" is a glm vector object specifying the Y plane to manipulate and is expressed
    //        as (0.0f, 1.0f, 0.0f).

    // "glm::normalize(glm::cross(Orientation, Up))" is a function set that returns correct values to 
    //      the camera position matrix. Rathing than using a "Horizontal" (1.0f, 0.0f, 0.0f) vector object, 
    //      normalize(cross()...) allows the user to navigate using the relative position coordinates of the camera.
    //------------------------------------------------------------------------------------------------------//
    //KEYBOARD BUTTON INPUTS
       
    //MOVE CAMERA FORWARD
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        Position += speed * Orientation;
    }
    //------------------------------------------------//
    //MOVE CAMERA BACK
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        Position += speed * -Orientation;
    } 
    //------------------------------------------------//
    //MOVE CAMERA UP
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        Position += speed * Up;
    }
    //------------------------------------------------//
    //MOVE CAMERA DOWN
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
    {
        Position += speed * -Up;
    }
    //------------------------------------------------//
    //MOVE CAMERA LEFT
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        Position += speed * -glm::normalize(glm::cross(Orientation, Up));
    }
    //------------------------------------------------//
    //MOVE CAMERA RIGHT
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        Position += speed * glm::normalize(glm::cross(Orientation, Up));
    }
    //------------------------------------------------//
    //INCREASE CAMERA SPEED
    if (glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        speed = 0.1f;
    }
    //------------------------------------------------//
    //RETURN TO NORMAL SPEED
    if (glfwGetKey(window,GLFW_KEY_LEFT_SHIFT) != GLFW_PRESS)
    {
        speed = 0.01f;
    }
    //------------------------------------------------------------------------------------------------//
    //MOUSE BUTTON PRESSES

    //LEFT MOUSE BUTTON
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
    {   
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
        //fixes camera jumping
        if (firstClick)
        {
            glfwSetCursorPos(window, width / 2, height / 2);
            firstClick = false;
        }

        //Establishes mouse position for camera rotation
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        float rotX = sensitivity * (float)(mouseY - (height/2)) / height;
        float rotY = sensitivity * (float)(mouseX - (width/2)) / width;
        //std::cout << rotY << " --- " << "100 * (" << mouseY << " - (" << width  << "/2)) / " << width << std::endl;
        //VERTICAL CAMERA MOVEMENT HANDLING
        glm::vec3 newOrientation = glm::rotate(Orientation, glm::radians(-rotX), glm::normalize(glm::cross(Orientation, Up)));
        //safety measure on vertical (X-axis) camera movement to prevent "barrel rolls"
        if (!(glm::angle(newOrientation, Up) <= glm::radians(5.0f)) || (glm::angle(newOrientation, -Up) <= glm::radians(5.0f)))
        {
            Orientation = newOrientation;
        }
        
        //HORIZONTAL CAMERA MOVEMENT HANDLING
        //no safety measures on camera movement side to side (y-axis)
        Orientation = glm::rotate(Orientation, glm::radians(-rotY), Up);
        
        glfwSetCursorPos(window, width / 2, height / 2);
    }

    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE)
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        firstClick = true;
    }
}