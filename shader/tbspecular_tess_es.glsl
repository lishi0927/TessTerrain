#version 410
layout(quads, equal_spacing, ccw) in;

in vec3 Position_ES_in[]; 
in vec3 outgrid[];

out vec2 uv;
out vec3 WorldPos0;
out vec4 grid;
out float AO;

uniform mat4 gWVP;
uniform mat4 gWorld;
uniform vec4 currentPos;

uniform sampler2D texHLevel;
uniform sampler2D texHLevel1;
uniform sampler2D vtex;
uniform sampler2D texHeightmap;

uniform sampler2D aotex;
uniform sampler2D texAOmap;
uniform sampler2D texaolevel;

const float VIEWCHUNKNUMBER = 16.0;
const float CHUNKNUMBER = 32.0;
const float CHUNKSIZE = 512.0;
const float MAXSCALE =  10 * CHUNKSIZE * VIEWCHUNKNUMBER / 4.0f;


void main()
{

	float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

	vec3 a = mix(Position_ES_in[1],Position_ES_in[0], u);
	vec3 b = mix(Position_ES_in[2],Position_ES_in[3], u);
	vec3 position = mix(a,b,v);
	uv = position.xz / MAXSCALE / 4.0 * VIEWCHUNKNUMBER / CHUNKNUMBER + currentPos.xy;
	
	float level = texture(texHLevel, uv).r;
	float ratio = fract(level);
	level -= ratio;
	grid.w = level / 6.0;
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
	position.y = mix(h, h1, ratio);

	gl_Position = gWVP * vec4(position, 1.0);

	WorldPos0 = (gWorld * vec4(position, 1.0)).xyz;

	grid.xy = outgrid[0].xy;

	AO = 0.0;

	if(grid.y == 1.0)
	{
	   level = texture(texaolevel, uv).r;
	   level *= 255;
	   scaleBias = textureLod(aotex, uv, level);
	   pCoor = uv * scaleBias.x + scaleBias.zw;
       AO = texture(texAOmap, pCoor).r;
	   grid.z = level / 6.0;
	}
}

