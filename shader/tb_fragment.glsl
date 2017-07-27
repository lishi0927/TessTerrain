#version 410 core

in vec2 UV_in;
in vec3 EyeDirection_worldspace;

struct BaseLight                                                                    
{                                                                                   
    vec3 Color;                                                                     
    float AmbientIntensity;                                                         
    float DiffuseIntensity;                                                         
};                                                                                  
                                                                                    
struct DirectionalLight                                                             
{                                                                                   
    BaseLight Base;                                                                 
    vec3 Direction;                                                                 
}; 

uniform sampler2D ntex;
uniform sampler2D normaltex;

uniform sampler2D btex;
uniform sampler2D blendtex;

uniform vec4 currentPos;

uniform sampler2D vtex;
uniform sampler2D texHeightmap;
uniform sampler2D texHLevel;
uniform sampler2D texHLevel1;

uniform int gShowTexture;

out vec4 color;

const float VIEWCHUNKNUMBER = 16.0;
const float MAXSCALE = 200.0 * VIEWCHUNKNUMBER / 4.0;
const float CHUNKNUMBER = 32.0;
const float CHUNKSIZE = 512.0;

const float maxAniso = 4;
const float maxAnisoLog2 = log2( maxAniso );
const float virtPagesWide = 32.0;
const float pageWidth = 512 + 8;
const float pageBorder = 4;
const float virtTexelsWide = virtPagesWide * ( pageWidth - 2 * pageBorder );

uniform DirectionalLight gDirectionalLight;                                                                 
uniform float gMatSpecularIntensity;                                                        
uniform float gSpecularPower; 
uniform mat4 gWorld;

const float maxheight = 200.0;
float heightscale = 1.0;

const vec2 size = vec2(2.0,0.0);
const ivec3 off = ivec3(-1,0,1);

float GetHeight(vec2 texCoords)
{
    vec2 uv = texCoords / MAXSCALE / 4.0 * VIEWCHUNKNUMBER / CHUNKNUMBER + currentPos.xy;
	
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

	return mix(h, h1, ratio);
}

vec2 ParallaxMapping(vec2 texCoords, vec3 viewDir)
{
//   if(abs(viewDir.x / viewDir.y) > 2 || abs(viewDir.z / viewDir.y) > 2) heightscale = 1.0;

   vec2 xz_var = viewDir.xz / viewDir.y;
   vec2 currenttexCoords = texCoords + maxheight * xz_var;
    
   const float minLayers = 50;
   const float maxLayers = 200;
   float numLayers = mix(maxLayers, minLayers, abs(dot(vec3(0, 1, 0), viewDir)));
   float layerDepth = maxheight / maxLayers;
   
   float currentLayerDepth = maxheight;
   vec2 p = xz_var * maxheight;
   vec2 deltaTexCoords = p / maxLayers;

   float currentDepthMapValue = GetHeight(currenttexCoords);

   while(currentDepthMapValue < currentLayerDepth)
   {
      currenttexCoords -= deltaTexCoords;
	  currentDepthMapValue = GetHeight(currenttexCoords);
	  currentLayerDepth -= layerDepth;
   }

   vec2 prevTexCoords = currenttexCoords + deltaTexCoords;

   float afterDepth =  currentLayerDepth - currentDepthMapValue;
   float beforeDepth = currentLayerDepth + layerDepth - GetHeight(prevTexCoords);

   float weight = afterDepth / (afterDepth - beforeDepth);
   vec2 finalTexCoords = prevTexCoords * weight + currenttexCoords * (1.0 - weight);

   return finalTexCoords;
}

vec4 CalcLightInternal(BaseLight Light, vec3 LightDirection, vec3 Normal)                  
{                                                                                           
    vec4 AmbientColor = vec4(Light.Color * Light.AmbientIntensity, 1.0f);
    float DiffuseFactor = dot(Normal, -LightDirection);                                     
                                                                                            
    vec4 DiffuseColor  = vec4(0, 0, 0, 0);                                                  
    vec4 SpecularColor = vec4(0, 0, 0, 0);                                                  
                                                                                            
    if (DiffuseFactor > 0.0) {                                                                
        DiffuseColor = vec4(Light.Color * Light.DiffuseIntensity * DiffuseFactor, 1.0f);
                                                                                            
        vec3 VertexToEye = normalize(EyeDirection_worldspace);                             
        vec3 LightReflect = normalize(reflect(LightDirection, Normal));                     
        float SpecularFactor = dot(VertexToEye, LightReflect);                                      
        if (SpecularFactor > 0.0) {                                                           
            SpecularFactor = pow(SpecularFactor, gSpecularPower);                               
            SpecularColor = vec4(Light.Color * gMatSpecularIntensity * SpecularFactor, 1.0f);
        }                                                                                   
    }                                                                                                                                                                           
    return AmbientColor + DiffuseColor + SpecularColor;                                  
} 

vec4 CalcDirectionalLight(vec3 Normal)                                                     
{                                                                                           
    return CalcLightInternal(gDirectionalLight.Base, gDirectionalLight.Direction, Normal);  
}

void main()
{
   vec3 E = EyeDirection_worldspace;
   vec2 uv = ParallaxMapping(UV_in, E);

   uv = uv / MAXSCALE / 4.0 * VIEWCHUNKNUMBER / CHUNKNUMBER + currentPos.xy;

   if(uv.x <= 0.001 || uv.x >= 0.999 || uv.y <= 0.001 || uv.y >= 0.999) discard;

//   E = normalize(E);

   vec2 tuv = uv * virtPagesWide;

   vec2 texcoords = uv * virtTexelsWide;

   vec2 dx = dFdx(texcoords);
   vec2 dy = dFdy(texcoords);

   float px = dot(dx, dx);
   float py = dot(dy, dy);

  float maxLod = 0.5 * log2(max(px, py));

   float anisoLOD = maxLod;
   float trilinearFraction = fract(anisoLOD);

   vec4 scaleBias1, scaleBias2;
   scaleBias1 = textureLod( btex, uv, anisoLOD-0.5);
   scaleBias2 = textureLod( btex, uv, anisoLOD+0.5);

   vec2 physCoords1 = uv * scaleBias1.x + scaleBias1.zw;
   vec2 physCoords2 = uv * scaleBias2.x + scaleBias2.zw;
   
   vec4 blend0 = texture(blendtex, physCoords1);
   vec4 blend0_1 = texture(blendtex, physCoords2);

   color = mix(blend0, blend0_1, trilinearFraction);
 
   scaleBias1 = textureLod( ntex, uv, anisoLOD - 0.5 );
   scaleBias2 = textureLod( ntex, uv, anisoLOD + 0.5 );
  
   physCoords1 = uv * scaleBias1.x + scaleBias1.zw;
   physCoords2 = uv * scaleBias2.x + scaleBias2.zw;

   vec4 normal0 = texture(normaltex, physCoords1);
   vec4 normal1 = texture(normaltex, physCoords2);

   normal0 = mix(normal0, normal1, trilinearFraction);
 
   vec3 Normal0 = normal0.xyz;
  
   vec3 Normal = normalize(Normal0);

if(gShowTexture == 1)
  {
    vec4 TotalLight = CalcDirectionalLight(Normal0);
    color = color * TotalLight;
  }
  else if(gShowTexture == 2)
  {
    vec3 Normal = (Normal0 + 1.0) / 2;
    color = vec4(Normal,1.0);
  }
   else if(gShowTexture == 0)
  {
    color = vec4(0.0,1.0,0.0,1.0);
  }
  else if(gShowTexture == 3)
  {
    color = vec4(1.0,0.0,0.0,1.0);
  }
}
