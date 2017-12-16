#version 410

const int MAX_SG_LIGHTS = 2;  

struct SG
{
	vec3 Axis; // lobe axis
	float Sharpness; // sharpness
	vec3 Amplitude; // amplitude
};

SG SGProduct(const SG x, const SG y)
{
   vec3 um = (x.Sharpness * x.Axis + y.Sharpness * y.Axis) /
                (x.Sharpness + y.Sharpness);
   float umLength = length(um);
   float lm = x.Sharpness + y.Sharpness;
 
   SG res;
   res.Axis = um * (1.0f / umLength);
   res.Sharpness = lm * umLength;
   res.Amplitude = x.Amplitude * y.Amplitude * exp(lm * (umLength - 1.0f));
 
   return res;
}


in vec2 uv;
//in vec3 WorldPos0;

uniform sampler2D vistex;
uniform sampler2D vispagetex;

uniform sampler2D ntex;
uniform sampler2D normaltex;

uniform sampler2D btex;
uniform sampler2D blendtex;

uniform int gNumSGLights; 
uniform SG gSGLights[MAX_SG_LIGHTS]; 

out vec4 color;

const float maxAniso = 4;
const float maxAnisoLog2 = log2( maxAniso );
const float virtPagesWide = 32.0;
const float pageWidth = 512 + 8;
const float pageBorder = 4;
const float virtTexelsWide = virtPagesWide * ( pageWidth - 2 * pageBorder );
const float pi = 3.1415926;

const float thetasize = 4;
const float phisize = 16;

uniform int gShowTexture;

void main()
{
   if(uv.x <= 0.003 || uv.x >= 0.997 || uv.y <= 0.003 || uv.y >= 0.997) discard;

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
   scaleBias1 = textureLod( btex, uv, anisoLOD - 0.5);
   scaleBias2 = textureLod( btex, uv, anisoLOD + 0.5);

   vec2 physCoords1 = uv * scaleBias1.x + scaleBias1.zw;
   vec2 physCoords2 = uv * scaleBias2.x + scaleBias2.zw;
   
   vec4 blend0 = texture(blendtex, physCoords1);
   vec4 blend0_1 = texture(blendtex, physCoords2);

   color = mix(blend0, blend0_1, trilinearFraction);

   vec4 scaleBias1_1 = textureLod( ntex, uv, anisoLOD - 0.5 );
   vec4 scaleBias2_1 = textureLod( ntex, uv, anisoLOD + 0.5 );
  
   physCoords1 = uv * scaleBias1_1.x + scaleBias1_1.zw;
   physCoords2 = uv * scaleBias2_1.x + scaleBias2_1.zw;

   vec4 normal0 = texture(normaltex, physCoords1);
   vec4 normal1 = texture(normaltex, physCoords2);

   normal0 = mix(normal0, normal1, trilinearFraction);
 
   vec3 Normal0 = vec3(normal0.x, normal0.y, normal0.z);
   Normal0 = 2.0 * Normal0 - 1.0;

   scaleBias1 = textureLod( vistex, uv, anisoLOD - 0.5 );
   scaleBias2 = textureLod( vistex, uv, anisoLOD + 0.5 );
  
   float visibility[int(phisize * thetasize)];
   ivec4 offsetx = ivec4(0,1,0,1);
   ivec4 offsety = ivec4(0,0,1,1);

   if(scaleBias1.y < 0.95)
   {
     physCoords1 = uv * scaleBias1.x + scaleBias1.zw;
     for(int k = 0; k < 4; k++)
     {
       vec4 vis0 = textureOffset(vispagetex, physCoords1,ivec2(offsetx[k],offsety[k]));
	   for(int j = 0; j < 4; j++)
       {
	      int result = int(vis0[j] * 65535);
	      for(int i = 0; i < 4; i++)
	      {
	       int remain = int(result) & 15;
	       visibility[4 * (4 * k + j) + i] = float(remain) / 15.0;
	       result = result >> 4;
	      }
	    }
      }
   }
   else
   {
     physCoords1 = uv * scaleBias1.x + scaleBias1.zw;
     vec4 vis0 = texture(vispagetex, physCoords1);
     for(int j = 0; j < 4; j++)
     {
	   int result = int(vis0[j] * 65535);
	   for(int i = 0; i < 16; i++)
	   {
	     int remain = result & 1;
	     visibility[16 * j + i] = float(remain);
	     result = result >> 1;
	    }
      }
    }

   if(scaleBias2.y < 0.95)
   {
     physCoords2 =  uv * scaleBias2.x + scaleBias2.zw;
     for(int k = 0; k < 4; k++)
     {
       vec4 vis0 = textureOffset(vispagetex, physCoords2,ivec2(offsetx[k],offsety[k]));
       for(int j = 0; j < 4; j++)
       {
	      int result = int(vis0[j] * 65535);
	      for(int i = 0; i < 4; i++)
	      {
	       int remain = int(result) & 15;
	       visibility[4 * j + i] = float(remain) / 15.0;
	       result = result >> 4;
	      }
	    }
      }
   }
   else
   {
     physCoords2 = uv * scaleBias2.x + scaleBias2.zw;
     vec4 vis0 = texture(vispagetex, physCoords2);
     for(int j = 0; j < 4; j++)
     {
	   int result = int(vis0[j] * 65535);
	   for(int i = 0; i < 16; i++)
	   {
	     int remain = result & 1;
	     visibility[16 * j + i] = float(remain);
	     result = result >> 1;
	    }
      }
   }

   vec3 Normal = normalize(Normal0);
   vec3 L = vec3(0);
   
   SG normalSG;
   normalSG.Axis = Normal;
   float length = Normal0.x * Normal0.x + Normal0.y * Normal0.y + Normal0.z * Normal0.z;
   length = sqrt(length);
   normalSG.Sharpness = length / (length + 2.133 * (1 - length)) * 2.133;
   normalSG.Amplitude =  1.170 * vec3(1.0, 1.0, 1.0);
   SG retSG;
  for(int i = 0; i < gNumSGLights; i++)
  {
   retSG = SGProduct(gSGLights[i], normalSG);
   float theta = acos(Normal.y);
   float phi = atan(Normal.z, Normal.x);
   mat3 T = mat3(sin(phi), sin(theta) * cos(phi), cos(theta) * cos(phi),
			0, cos(theta), sin(theta),
		-cos(phi), sin(phi) * sin(theta), cos(theta) * sin(phi));
	  
  for(int j = 0; j < thetasize; j++)
   for(int k = 0; k < phisize; k++)
	  {
	    float mintheta = -pi, maxtheta = pi, minphi = pi, maxphi = -pi;
	    for(int m = 0; m < 4; m++)
	    {
	      float vectheta = pi / thetasize / 2 * (j+offsetx[m]);
	      float vecphi =  2 * pi / phisize * (k + offsety[m]) - pi;
	      vec3 vector = vec3(sin(vectheta) * cos(vecphi), cos(vectheta), sin(vectheta) * sin(vecphi));
	      vec3 newvector = T * vector;
	      vectheta = acos(newvector.y);
	      vecphi = atan(newvector.z, newvector.x);
	      if(vectheta > maxtheta)maxtheta = vectheta;
	      if(vectheta < mintheta)mintheta = vectheta;
	      if(vecphi > maxphi)maxphi = vecphi;
	      if(vecphi < minphi)minphi = vecphi;
	    }
	   int minthetaindex = int(mintheta / (pi / thetasize / 2));
	   int maxthetaindex = int(maxtheta / (pi / thetasize / 2));
	   int minphiindex = int((minphi + pi) / (pi * 2 / phisize));
	   int maxphiindex = int((maxphi + pi) / (pi * 2 / phisize));
	   float weight = 0.0;
	   for(int m = minthetaindex; m <= maxthetaindex; m++)
	   {
	     for(int n = minphiindex; n <= maxphiindex; n++)
	     {
	       if(m < thetasize && n < phisize)
	       {
	         weight = weight + visibility[int(m * phisize + n)];
	       }
	      }
	    }
	    weight = weight / (maxthetaindex - minthetaindex + 1) / (maxphiindex - minphiindex + 1);
	    L = L + weight * 2 * pi / phisize * retSG.Amplitude / retSG.Sharpness * (exp(cos(j*pi/2/thetasize)-1)-exp(cos((j+1)*pi/2/thetasize)-1));
	 }
    }
 if(gShowTexture == 0)
  {
	color = color * vec4(L,1.0);
  }
  else if(gShowTexture == 1)
  {
     color = color;
  }
  else if(gShowTexture == 2)
  {
	float newvisibility = 0.0;
    for(int i = 0; i < thetasize; i++)
		for(int j = 0; j < phisize; j++)
		{
		  newvisibility = newvisibility + visibility[int(i*phisize+j)];
		}
		newvisibility = newvisibility / thetasize /  phisize;
		color = vec4(newvisibility,0.0,0.0,0.0);
  }
}