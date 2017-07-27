#version 410
layout(quads, equal_spacing, ccw) in;

in vec3 Position_ES_in[]; 

out vec2 uv;
out float flag;

uniform mat4 gWVP;
uniform vec4 currentPos;

uniform sampler2D vtex;
uniform sampler2D texHeightmap;
uniform sampler2D texHLevel;
uniform sampler2D texHLevel1;

uniform float blocksize;

const float VIEWCHUNKNUMBER = 16.0;
const float MAXSCALE = 200.0 * VIEWCHUNKNUMBER / 4.0;
const float CHUNKNUMBER = 32.0;
const float CHUNKSIZE = 512.0;

void main()
{
	float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

	flag = float(Position_ES_in[0].y / blocksize);

	vec3 a = mix(Position_ES_in[1],Position_ES_in[0], u);
	vec3 b = mix(Position_ES_in[2],Position_ES_in[3], u);
	vec3 position = mix(a,b,v);
	uv = position.xz / MAXSCALE / 4.0 * VIEWCHUNKNUMBER / CHUNKNUMBER + currentPos.xy;
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
	h *= 6553.5f;
	position.y = mix(h, h1, ratio);

	gl_Position = gWVP * vec4(position, 1.0);
}

