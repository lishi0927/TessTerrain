#version 410

layout (location = 0) in vec3 position;

out vec2 UV_in;
out vec3 EyeDirection_worldspace;

uniform mat4 gWVP;

uniform mat4 gWorld;
uniform vec3 gEyeWorldPos;

void main()
{
    UV_in = position.xz;

    gl_Position = gWVP * vec4(position, 1.0);
   
   vec3 Position_worldspace =  (gWorld * vec4(position, 1.0)).xyz;

   EyeDirection_worldspace = gEyeWorldPos - Position_worldspace;
}
