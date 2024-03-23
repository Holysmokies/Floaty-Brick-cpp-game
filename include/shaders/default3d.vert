#version 330 core

//Gives information about where values are in the vertex array
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec3 aColor;
layout (location = 3) in vec2 aTex;

//OUTPUTS
out vec3 currentPosition;
out vec3 normal;
out vec3 color;
out vec2 texCoord;

//UNIFORMS
uniform mat4 cameraMatrix;
uniform mat4 model;
uniform mat4 translation;
uniform mat4 rotation;
uniform mat4 scale;

void main()
{
   //                                           v ? remove ?
   currentPosition = vec3(model * translation * -rotation * scale * vec4(aPos, 1.0f));
   normal = aNormal;
   color = aColor;
   //                   ? remove ?
   texCoord = mat2(0.0, -1.0, 1.0, 0.0) * aTex;
   

   //MANDATORY OUTPUT "gl_Position"
   gl_Position = cameraMatrix * vec4(currentPosition, 1.0);
}