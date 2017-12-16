#version 410

layout(vertices = 4) out;

// attributes of the input CPs                                                                  
in vec3 Position_CS_in[];

// attributes of the output CPs                                                                 
out vec3 Position_ES_in[];


void main()
{
   // Set the control points of the output patch                                               
  Position_ES_in[gl_InvocationID] = Position_CS_in[gl_InvocationID];
           
  gl_TessLevelOuter[0] = 32;
  gl_TessLevelOuter[1] = 32;
  gl_TessLevelOuter[2] = 32;
  gl_TessLevelOuter[3] = 32;
  gl_TessLevelInner[0] = 32;
  gl_TessLevelInner[1] = 32;
}