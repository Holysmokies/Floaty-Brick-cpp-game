#version 330 core

out vec4 FragColor;

//Inputs from vertex shader output 
in vec3 currentPosition;
in vec3 normal;
in vec3 color;
in vec2 texCoord;

//TEXTURE SAMPLER UNIFORMS
uniform sampler2D diffuse1;
uniform sampler2D specular1;

//NORMAL UNIFORMS
uniform vec4 lightColor;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;


vec4 point_light()
{  
   vec3 light_vector = lightPosition - currentPosition;
   float distance = length(light_vector);
   float a = 0.1;
   float b = 0.01;
   //    1/(a*b^2+b*d+a)
   float intensity = 1.0f / (a * distance * distance + b * distance + 1.0f);


   //Simulates light "bouncing" in real life but is much less intensive to calculate than proper ray tracing
   float ambient = 0.2f;

   // NORMALIZING makes it such that all values return on a scale of 0-1
   vec3 normal_values = normalize(normal);
   vec3 light_Direction = normalize(lightPosition - currentPosition);

   //DIFFUSE LIGHT
   // The larger the angle between two previously calculated vectors = less intense the light
   // dot(a, b) is equal to the COSINE of the angle between them without including negative values max(..., 0.0f).
   float diffuse = max(dot(normal_values, light_Direction), 0.0f);

   //SPECULAR LIGHT 
   float spec_light = 0.5f;
   vec3 view_direction = normalize(cameraPosition - currentPosition);
   vec3 reflection_direction = reflect(-light_Direction, normal_values);
   float spec_amount = pow(max(dot(view_direction, reflection_direction), 0.0f), 16);
   float specular = spec_amount * spec_light;

               //Assigns color and light                                  || Adds specular lighting (REQUIRES SPEC MAP)                                     
   return (texture(diffuse1, texCoord)  * (diffuse * intensity + ambient) + texture(specular1, texCoord).r * specular * intensity) * lightColor;
}


vec4 directional_light()
{
   //AMBIENT LIGHT
   float ambient = 0.2f;

   //DIFFUSE LIGHT
   vec3 normal_values = normalize(normal);
   vec3 light_Direction = normalize(vec3(1.0f, 1.0f, 0.0f));
   float diffuse = max(dot(normal_values, light_Direction), 0.0f);

   //SPECULAR LIGHT 
   float spec_light = 0.5f;
   vec3 view_direction = normalize(cameraPosition - currentPosition);
   vec3 reflection_direction = reflect(-light_Direction, normal_values);
   float spec_amount = pow(max(dot(view_direction, reflection_direction), 0.0f), 16);
   float specular = spec_amount * spec_light;
                                   
   return texture(diffuse1, texCoord) * lightColor * (diffuse + ambient) + texture(specular1, texCoord).r * specular ;
}


vec4 spot_light()
{
   float outer_cone = 0.90f;
   float inner_cone = 0.95f;

   //AMBIENT LIGHT
   float ambient = 0.2f;

   //DIFFUSE LIGHT
   vec3 normal_values = normalize(normal);
   vec3 light_Direction = normalize(lightPosition - currentPosition);
   float diffuse = max(dot(normal_values, light_Direction), 0.0f);

   //SPECULAR LIGHT 
   float spec_light = 0.5f;
   vec3 view_direction = normalize(cameraPosition - currentPosition);
   vec3 reflection_direction = reflect(-light_Direction, normal_values);
   float spec_amount = pow(max(dot(view_direction, reflection_direction), 0.0f), 16);
   float specular = spec_amount * spec_light;

   //Calculates intensity of light based on the angle of the position to the source
   float angle = dot(vec3(0.0f, -1.0f, 0.0f), -light_Direction);
   float intensity = clamp((angle - outer_cone) / (inner_cone - outer_cone), 0.0f, 1.0f);

   return (texture(diffuse1, texCoord) * (diffuse * intensity + ambient) + texture(specular1, texCoord).r * specular * intensity) * lightColor;
}

void main()
{                                    
   FragColor = point_light();
}