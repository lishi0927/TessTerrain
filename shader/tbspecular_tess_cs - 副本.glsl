#version 410

layout(vertices = 4) out;


// attributes of the input CPs                                                                  
in vec3 Position_CS_in[];

// attributes of the output CPs                                                                 
out vec3 Position_ES_in[];
out vec3 outgrid[];

uniform sampler2D texHLevel;
uniform sampler2D texHLevel1;
uniform sampler2D vtex;
uniform sampler2D texHeightmap;
uniform sampler2D Satmap;

uniform sampler2D texaolevel;
uniform sampler2D texaomaxdiff;

uniform mat4 gWVP;
uniform vec4 currentPos;

const float VIEWCHUNKNUMBER = 16.0;
const float CHUNKNUMBER = 32.0;
const float CHUNKSIZE = 512.0;
const float MAXSCALE =  10 * CHUNKSIZE * VIEWCHUNKNUMBER / 4.0f;

const float htop = 40000.0;
const float ep = 60.0;

const float WIDTH = 1024;
const float HEIGHT = 720;

void main()
{
  // Set the control points of the output patch                                               
    Position_ES_in[gl_InvocationID] = Position_CS_in[gl_InvocationID];
            
    // Calculate the tessellation levels
  float lx = Position_ES_in[0].x/ MAXSCALE / 4.0 * VIEWCHUNKNUMBER / CHUNKNUMBER + currentPos.x;
  float rx = Position_ES_in[1].x/ MAXSCALE / 4.0 * VIEWCHUNKNUMBER / CHUNKNUMBER + currentPos.x;
  float ly = Position_ES_in[0].z/ MAXSCALE / 4.0 * VIEWCHUNKNUMBER / CHUNKNUMBER + currentPos.y;
  float ry = Position_ES_in[2].z/ MAXSCALE / 4.0 * VIEWCHUNKNUMBER / CHUNKNUMBER + currentPos.y;
 
  lx = clamp(lx, 0.0, 1.0);
  ly = clamp(ly, 0.0, 1.0);
  rx = clamp(rx, 0.0, 1.0);
  ry = clamp(ry, 0.0, 1.0);

  int blx, bly, brx, bry, mlx, mly, mrx, mry;
  blx = int(floor(lx * CHUNKNUMBER));
  bly = int(floor(ly * CHUNKNUMBER));
  mlx = int(lx * CHUNKNUMBER * CHUNKSIZE - blx * CHUNKSIZE);
  mly = int(ly * CHUNKNUMBER * CHUNKSIZE - bly * CHUNKSIZE);

  brx = int(floor(rx * CHUNKNUMBER));
  bry = int(floor(ry * CHUNKNUMBER));
  mrx = int(rx* CHUNKNUMBER * CHUNKSIZE - brx * CHUNKSIZE);
  mry = int(ry* CHUNKNUMBER * CHUNKSIZE - bry * CHUNKSIZE);

  double satvalue = 0;
  int grid = 0;
  int maxaolevel = 6;
  float maxaoerror = 0;

   for (int x = blx; x <= brx; x++)
    for (int y = bly; y <= bry; y++) {
	  float deltax = x / CHUNKNUMBER;
	  float deltay = y / CHUNKNUMBER;

	  int level = int(texture(texaolevel, vec2(deltax,deltay)).r * 255);
	  if(level < maxaolevel) maxaolevel = level;

	  float aoerror = texture(texaomaxdiff, vec2(deltax,deltay)).r;
	  if(maxaoerror < aoerror) maxaoerror = aoerror;
	}

  for (int x = blx; x <= brx; x++)
    for (int y = bly; y <= bry; y++) {
	
	  float deltax = x / CHUNKNUMBER;
	  float deltay = y / CHUNKNUMBER;

	  int level = int(texture(texHLevel, vec2(deltax,deltay)).r);
	  
	  int templevel = 6 - level;
	  if(grid < templevel) 
	  {
	   grid = templevel;
	   if(grid == 6)
	     break;
	  }

	  int localLx, localLy, localRx, localRy;
	  if(x == blx) localLx = int(lx * CHUNKNUMBER * CHUNKSIZE);
	  else localLx = int(x * CHUNKSIZE);

	  if(x == brx) localRx = int(rx * CHUNKNUMBER * CHUNKSIZE);
	  else localRx = int((x + 1) * CHUNKSIZE - 1);

	  if(y == bly) localLy = int(ly * CHUNKNUMBER * CHUNKSIZE);
	  else localLy = int(y * CHUNKSIZE);

	  if(y == bry) localRy = int(ry * CHUNKNUMBER * CHUNKSIZE);
	  else localRy = int((y + 1) * CHUNKSIZE - 1);


	  localLx = int(x * CHUNKSIZE) + int((localLx - int(int(x / pow(2.0, level)) * pow(2.0,level)) * CHUNKSIZE) / pow(2.0,level));
	  localRx = int(x * CHUNKSIZE) + int((localRx - int(int(x / pow(2.0, level)) * pow(2.0,level)) * CHUNKSIZE) / pow(2.0,level));
	  localLy = int(y * CHUNKSIZE) + int((localLy - int(int(y / pow(2.0, level)) * pow(2.0,level)) * CHUNKSIZE) / pow(2.0,level));
	  localRy = int(y * CHUNKSIZE) + int((localRy - int(int(y / pow(2.0, level)) * pow(2.0,level)) * CHUNKSIZE) / pow(2.0,level));

	  float yposbottom =  1.0 * localLy / float(CHUNKSIZE * CHUNKNUMBER);
	  float ypostop = 1.0 * localRy / float(CHUNKSIZE * CHUNKNUMBER);
	  float xposleft = 1.0 * localLx / float(CHUNKSIZE * CHUNKNUMBER);
	  float xposright = 1.0 * localRx / float(CHUNKSIZE * CHUNKNUMBER);

	  vec4 scaleBias = textureLod(vtex, vec2(xposleft,yposbottom), level);
	  vec2 pCoor = vec2(xposleft,yposbottom) * scaleBias.x + scaleBias.zw;
	  vec4 sat = texture(Satmap, pCoor);
	  double lbret = double(sat.r / 256.0) + double(sat.g / 256.0 / 256.0) + double(sat.b / 256.0 / 256.0 / 256.0) + double(sat.a / 256.0 / 256.0 / 256.0 / 256.0);

	  scaleBias = textureLod(vtex, vec2(xposright,yposbottom), level);
	  pCoor = vec2(xposright,yposbottom) * scaleBias.x + scaleBias.zw;
	  sat = texture(Satmap, pCoor);
	  double rbret = double(sat.r / 256.0) + double(sat.g / 256.0 / 256.0) + double(sat.b / 256.0 / 256.0 / 256.0) + double(sat.a / 256.0 / 256.0 / 256.0 / 256.0);

	  scaleBias = textureLod(vtex, vec2(xposleft,ypostop), level);
	  pCoor = vec2(xposleft,ypostop) * scaleBias.x + scaleBias.zw;
	  sat = texture(Satmap, pCoor);
	  double ltret = double(sat.r / 256.0) + double(sat.g / 256.0 / 256.0) + double(sat.b / 256.0 / 256.0 / 256.0) + double(sat.a / 256.0 / 256.0 / 256.0 / 256.0);

	  scaleBias = textureLod(vtex, vec2(xposright,ypostop), level);
	  pCoor = vec2(xposright,ypostop) * scaleBias.x + scaleBias.zw;
	  sat = texture(Satmap, pCoor);
	  double rtret = double(sat.r / 256.0) + double(sat.g / 256.0 / 256.0) + double(sat.b / 256.0 / 256.0 / 256.0) + double(sat.a / 256.0 / 256.0 / 256.0 / 256.0);

	  double ret = rtret - ltret - rbret + lbret;

	  float retf = float(ret);
	  retf = pow(retf, 1.0 / ep);
	  retf = retf + 1;
	  retf = retf * htop;
	  retf = retf / pow(2.0, level);

	  if(satvalue < retf) satvalue = retf;
	}

if(grid < 6)
{
	vec4 center = vec4(0.25 * (Position_ES_in[0] + Position_ES_in[1] + Position_ES_in[2] + Position_ES_in[3]), 1.0);
	float Dp = sqrt(center.x * center.x + center.z * center.z);
	vec4 maxside = vec4(Position_ES_in[2] - Position_ES_in[0],1.0);
	float s = sqrt(maxside.x * maxside.x + maxside.z * maxside.z) / 2.0;
	float d = 1 - s / Dp;
	if(d > 0)
	{
	  for(int i = 0; i < 6 - grid; i++)
	  {
	    center.y = -currentPos.z;
	    center = center * d;
	    center.y += currentPos.z;
	    center.w = 1.0;
	    vec4 p = center;
	    p.y = center.y +  float(satvalue / 2);
	    center = gWVP * center;
		p = gWVP * center;
		float xPixel = (abs(center.x - p.x)) * WIDTH;
		float yPixel = (abs(center.y - p.y)) * HEIGHT;
		float flag = sqrt(xPixel * xPixel + yPixel * yPixel);
		if(flag < 2)
		{
		  break;
		}
		 grid = grid + 1;
		 satvalue = satvalue / 2.0;
	  }
	  grid = int(pow(2.0,grid));
	}
	else
	{
	  grid = 64;
	}
}
else
{
  grid = 64;
}

  float aoflag = 0.0; // aoflag : 1.0表示在vertex shader，0.0在fragment shader；

   grid = clamp(grid, 8, 64);

  if(grid >= pow(2.0, (6 - maxaolevel) + int(maxaoerror / 0.2))) aoflag = 1.0;
 
  outgrid[gl_InvocationID] = vec3(1.0 * grid / 64, aoflag, 0);

  gl_TessLevelOuter[0] = Position_ES_in[1].y;
  gl_TessLevelOuter[1] = Position_ES_in[2].y;
  gl_TessLevelOuter[2] = Position_ES_in[0].y;
  gl_TessLevelOuter[3] = Position_ES_in[3].y;
  gl_TessLevelInner[0] = grid;
  gl_TessLevelInner[1] = grid;
}