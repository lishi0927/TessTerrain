#version 410

layout(location = 0)in vec3 Position_VS_in;  

out vec3 Position_CS_in;

void main()
{
  Position_CS_in = Position_VS_in; 
}