#version 410

in vec2 uv;
in vec3 WorldPos0;
in float grid;

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

out vec4 color;

const float maxAniso = 4;
const float maxAnisoLog2 = log2( maxAniso );
const float virtPagesWide = 32.0;
const float pageWidth = 512 + 8;
const float pageBorder = 4;
const float virtTexelsWide = virtPagesWide * ( pageWidth - 2 * pageBorder );

uniform DirectionalLight gDirectionalLight;                                                              
uniform vec3 gEyeWorldPos;                                                                  
uniform float gMatSpecularIntensity;                                                        
uniform float gSpecularPower; 

uniform int gShowTexture;

vec4 CalcLightInternal(BaseLight Light, vec3 LightDirection, vec3 Normal)                  
{                                                                                           
    vec4 AmbientColor = vec4(Light.Color * Light.AmbientIntensity, 1.0f);
    float DiffuseFactor = dot(Normal, -LightDirection);                                     
                                                                                            
    vec4 DiffuseColor  = vec4(0, 0, 0, 0);                                                  
    vec4 SpecularColor = vec4(0, 0, 0, 0);                                                  
                                                                                            
    if (DiffuseFactor > 0.0) {                                                                
        DiffuseColor = vec4(Light.Color * Light.DiffuseIntensity * DiffuseFactor, 1.0f);
                                                                                            
        vec3 VertexToEye = normalize(gEyeWorldPos - WorldPos0);                             
        vec3 LightReflect = normalize(reflect(LightDirection, Normal));                     
        float SpecularFactor = dot(VertexToEye, LightReflect);                                      
        if (SpecularFactor > 0.0) {                                                           
            SpecularFactor = pow(SpecularFactor, gSpecularPower);                               
            SpecularColor = vec4(Light.Color * gMatSpecularIntensity * SpecularFactor, 1.0f);
        }                                                                                   
    }                                                                                  
                                                                                          
    return (AmbientColor +  (DiffuseColor + SpecularColor));                                  
} 

vec4 CalcDirectionalLight(vec3 Normal)                                                     
{                                                                                           
    return CalcLightInternal(gDirectionalLight.Base, gDirectionalLight.Direction, Normal);  
}                                                                                           
                                                                                            

void main()
{
   if(uv.x <= 0.001 || uv.x >= 0.999 || uv.y <= 0.001 || uv.y >= 0.999) discard;

   vec3 Normal0 = vec3(0.0,0.0,0.0);

   if(gShowTexture == 1)
  {
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
 
   Normal0 = normal0.xyz;
  
   vec3 Normal = normalize(Normal0);

   vec4 TotalLight = CalcDirectionalLight(Normal);

   color = color * TotalLight;
  }
  else if(gShowTexture == 2)
  {
    vec3 Normal = (Normal0 + 1.0) / 2;
    color = vec4(Normal,1.0);
  }
  else if(gShowTexture == 3)
  {
    float ans = grid;
    color = vec4(ans,ans,ans,1.0);
  }
   else if(gShowTexture == 0)
  {
    color = vec4(1.0,0.0,0.0,1.0);
  }
}