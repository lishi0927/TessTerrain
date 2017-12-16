#version 410

layout(vertices = 4) out;

uniform vec4 currentPos;
uniform sampler2D vtex;
uniform sampler2D texHeightmap;
uniform sampler2D texHLevel;
uniform sampler2D texHLevel1;

// attributes of the input CPs                                                                  
in vec3 Position_CS_in[];

// attributes of the output CPs                                                                 
out vec3 Position_ES_in[];

const float VIEWCHUNKNUMBER = 16.0;
const float CHUNKNUMBER = 32.0;
const float CHUNKSIZE = 512.0;
const float MAXSCALE =  10 * CHUNKSIZE * VIEWCHUNKNUMBER / 4.0f;


void main()
{
   // Set the control points of the output patch                                               
    Position_ES_in[gl_InvocationID] = Position_CS_in[gl_InvocationID];

	gl_TessLevelOuter[0] = 1;
    gl_TessLevelOuter[1] = Position_ES_in[0].y;
    gl_TessLevelOuter[2] = 1;
    gl_TessLevelOuter[3] = Position_ES_in[0].y;
    gl_TessLevelInner[0] = Position_ES_in[0].y;
    gl_TessLevelInner[1] = 1;

	if(gl_InvocationID == 2)
	{
	 vec2 uv = Position_ES_in[2].xz / MAXSCALE / 4.0 * VIEWCHUNKNUMBER / CHUNKNUMBER + currentPos.xy;

	 float level = texture(texHLevel, uv).r;
	 float ratio = fract(level);
	 level -= ratio;
	 vec4 scaleBias = textureLod(vtex, uv, level);
	 level = texture(texHLevel1, uv).r;
	 level *= 255;
	 vec4 scaleBias1 = textureLod(vtex, uv, level);
	 vec2 pCoor = uv * scaleBias.x + scaleBias.zw;
	 vec2 pCoor1 = uv * scaleBias1.x + scaleBias1.zw;
     float h = texture(texHeightmap, pCoor).r;
	 float h1 = texture(texHeightmap, pCoor1).r;

	 h1 *= 6553.5f;
	 h  *= 6553.5f;

      Position_ES_in[gl_InvocationID].y = mix(h, h1, ratio);
	}
	else if(gl_InvocationID == 3)
	{
	vec2 uv = Position_ES_in[3].xz / MAXSCALE / 4.0 * VIEWCHUNKNUMBER / CHUNKNUMBER + currentPos.xy;

	 float level = texture(texHLevel, uv).r;
	 float ratio = fract(level);
	 level -= ratio;
	 vec4 scaleBias = textureLod(vtex, uv, level);
	 level = texture(texHLevel1, uv).r;
	 level *= 255;
	 vec4 scaleBias1 = textureLod(vtex, uv, level);
	 vec2 pCoor = uv * scaleBias.x + scaleBias.zw;
	 vec2 pCoor1 = uv * scaleBias1.x + scaleBias1.zw;
     float h = texture(texHeightmap, pCoor).r;
	 float h1 = texture(texHeightmap, pCoor1).r;

	 h1 *= 6553.5f;
	 h  *= 6553.5f;

      Position_ES_in[gl_InvocationID].y = mix(h, h1, ratio);
	}
	else if(gl_InvocationID == 0)
	{
		Position_ES_in[gl_InvocationID].y = 0;
	}
}
