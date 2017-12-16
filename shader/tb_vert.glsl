#version 410

layout (location = 0) in vec3 position;

out vec2 UV_in;
out vec3 EyeDirection_worldspace;
out float AO;

uniform mat4 gWVP;

uniform mat4 gWorld;
uniform vec3 gEyeWorldPos;

uniform sampler2D aotex;
uniform sampler2D texAOmap;
uniform sampler2D texaolevel;

uniform vec4 currentPos;

const float VIEWCHUNKNUMBER = 16.0;
const float CHUNKNUMBER = 32.0;
const float CHUNKSIZE = 512.0;
const float MAXSCALE =  10 * CHUNKSIZE * VIEWCHUNKNUMBER / 4.0f;

void main()
{
    UV_in = position.xz;

	AO = -1;

	vec3 newposition = position;

	if(newposition.y == 1.0)
	{
	    newposition.y = 0.0;
		vec2 uv = position.xz / MAXSCALE / 4.0 * VIEWCHUNKNUMBER / CHUNKNUMBER + currentPos.xy;
	    float level = texture(texaolevel, uv).r;
	    level *= 255;
	    vec4 scaleBias = textureLod(aotex, uv, level);
	    vec2 pCoor = uv * scaleBias.x + scaleBias.zw;
        AO = texture(texAOmap, pCoor).r;
	}

    gl_Position = gWVP * vec4(newposition, 1.0);
   
    vec3 Position_worldspace =  (gWorld * vec4(newposition, 1.0)).xyz;

    EyeDirection_worldspace = gEyeWorldPos - Position_worldspace;
}
