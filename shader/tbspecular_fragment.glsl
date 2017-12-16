#version 410

in vec2 uv;
in vec3 WorldPos0;
in vec4 grid;
in float AO;

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

uniform sampler2D aotex;
uniform sampler2D texAOmap;
uniform sampler2D texaolevel;

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
uniform int gShowAO;

vec4 CalcLightInternal(BaseLight Light, vec3 LightDirection, vec3 Normal,float AOvalue, vec3 M)                  
{                                                                                           
    vec4 AmbientColor = vec4(Light.Color * Light.AmbientIntensity, 1.0f);
    float DiffuseFactor = dot(Normal, -LightDirection);                                     
                                                                                            
    vec4 DiffuseColor  = vec4(0, 0, 0, 0);                                                  
    vec4 SpecularColor = vec4(0, 0, 0, 0);                                                  
                                                                                            
    if (DiffuseFactor > 0.0) {                                                                
        DiffuseColor = vec4(Light.Color * Light.DiffuseIntensity * DiffuseFactor, 1.0f);
                                                                                            
        vec3 VertexToEye = normalize(gEyeWorldPos - WorldPos0);                             
//      vec3 LightReflect = normalize(reflect(LightDirection, Normal));  
        vec3 halfvector = normalize(-LightDirection + VertexToEye);             
        float SpecularFactor = dot(Normal, halfvector);                                      
        if (SpecularFactor > 0.0) { 
//			vec2 h = halfvector.xz / halfvector.y - M.xy;
//		    float sigma = M.z - M.x * M.x - M.y * M.y + 1.0 /  gSpecularPower;
//			SpecularFactor = exp(- 0.5 * ((h.x - M.x) * (h.x - M.x) + (h.y - M.y) * (h.y - M.y)) / sigma) ;		                                                      
          SpecularFactor = pow(SpecularFactor, gSpecularPower);                      
            SpecularColor = vec4(Light.Color * gMatSpecularIntensity * SpecularFactor, 1.0f);
        }                                                                                   
    }                                                                                  
                                                                                          
    return (AmbientColor * (1.0 - gShowAO * (AOvalue / 2.0)) +  (DiffuseColor + SpecularColor));                                  
} 

vec4 CalcDirectionalLight(vec3 Normal,float AOvalue, vec3 M)                                                     
{                                                                                           
    return CalcLightInternal(gDirectionalLight.Base, gDirectionalLight.Direction, Normal,AOvalue, M);  
}                                                                                           
                                                                                            

void main()
{
   if(uv.x <= 0.003 || uv.x >= 0.997 || uv.y <= 0.003 || uv.y >= 0.997) discard;

   vec3 Normal0 = vec3(0.0,0.0,0.0);
   
   vec2 tuv = uv * virtPagesWide;

   vec2 texcoords = uv * virtTexelsWide;

   vec2 dx = dFdx(texcoords);
   vec2 dy = dFdy(texcoords);

   float px = dot(dx, dx);
   float py = dot(dy, dy);

   float maxLod = 0.5 * log2(max(px, py));
// float minLod = 0.5 * log2( min( px, py ) );
// float anisoLOD = maxLod - min( maxLod - minLod, maxAnisoLog2 );

   float anisoLOD = maxLod;
   float trilinearFraction = fract(anisoLOD);

   vec4 scaleBias1, scaleBias2;
   scaleBias1 = textureLod( btex, uv, anisoLOD - 0.5);
   scaleBias2 = textureLod( btex, uv, anisoLOD + 0.5);

   vec2 physCoords1 = uv * scaleBias1.x + scaleBias1.zw;
   vec2 physCoords2 = uv * scaleBias2.x + scaleBias2.zw;
   
   vec4 blend0 = texture(blendtex, physCoords1);
   vec4 blend0_1 = texture(blendtex, physCoords2);

   color = mix(blend0, blend0_1, trilinearFraction);
 
   scaleBias1 = textureLod( ntex, uv, anisoLOD - 0.5 );
   scaleBias2 = textureLod( ntex, uv, anisoLOD + 0.5 );
  
   physCoords1 = uv * scaleBias1.x + scaleBias1.zw;
   physCoords2 = uv * scaleBias2.x + scaleBias2.zw;

   vec3 normal0 = texture(normaltex, physCoords1).rgb;
   vec3 normal1 = texture(normaltex, physCoords2).rgb;

  
   normal0 = mix(normal0, normal1, trilinearFraction);


 
//   Normal0 = vec3((2 * normal0.xy - 1.0) * 40, 1.0);
//   Normal0 = vec3(normal0.x, 1.0, normal0.z);
     Normal0 = vec3(normal0.x, normal0.y, normal0.z);
     Normal0 = 2.0 * Normal0 - 1.0;

// Normal0 = normal0.xyz;
 //vec3 M = vec3(Normal0.xz, normal0.z);

   float aolevelflag = grid.z;

   float AOvalue = AO * grid.y;

   if(grid.y == 0.0)
	{
	   float level = texture(texaolevel, uv).r;
	   level *= 255;
	   aolevelflag = level / 6.0;
	   vec4 scaleBias = textureLod(aotex, uv, level);
	   vec2 pCoor = uv * scaleBias.x + scaleBias.zw;
           AOvalue = texture(texAOmap, pCoor).r;
	}

  if(gShowTexture == 1)
  {
   vec3 Normal = normalize(Normal0);

   vec4 TotalLight = CalcDirectionalLight(Normal, AOvalue, vec3(0.0,0.0,0.0));

   color = color * TotalLight;
  }
  else if(gShowTexture == 2)
  {
   vec3 Normal = normalize(Normal0);
   Normal = (Normal + 1.0) / 2;
   color = vec4(normal1,1.0);
  }
  else if(gShowTexture == 3)
  {
    float ans = grid.x;

    color = vec4(ans,ans,ans,1.0);
  }
   else if(gShowTexture == 0)
  {
     color = color;
  }
    else if(gShowTexture == 4)
  {
     float ans = grid.y;

    color = vec4(ans,ans,ans,1.0);
  }
  else if(gShowTexture == 5)
  {
     
    float ans = aolevelflag;

    color = vec4(ans,ans,ans,1.0);
  }
   else if(gShowTexture == 6)
  {
     
    float ans =  grid.w;

    color = vec4(ans,ans,ans,1.0);
  }
}