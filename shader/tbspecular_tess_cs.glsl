#version 410

layout(vertices = 4) out;
uniform sampler2D vtex;
uniform sampler2D Satmap;

// attributes of the input CPs                                                                  
in vec3 Position_CS_in[];

// attributes of the output CPs                                                                 
out vec3 Position_ES_in[];
out vec3 outgrid[];

const float VIEWCHUNKNUMBER = 16.0;
const float MAXSCALE = 200.0 * VIEWCHUNKNUMBER / 4.0;
const float CHUNKNUMBER = 32.0;
const float CHUNKSIZE = 512.0;
const double htop = 40000.0;
const double ep = 60.0;

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

  for (int x = blx; x <= brx; x++)
    for (int y = bly; y <= bry; y++) {
	int localLx, localLy, localRx, localRy;
	  if(x == blx) localLx = lx * CHUNKNUMBER * CHUNKSIZE;
	  else localLx = x * CHUNKSIZE;

	  if(x == brx) localRx = rx * CHUNKNUMBER * CHUNKSIZE;
	  else localRx = (x + 1) * CHUNKSIZE - 1;

	  if(y == bly) localLy = ly * CHUNKNUMBER * CHUNKSIZE;
	  else localLy = y * CHUNKSIZE;

	  if(y == bry) localRy = ry * CHUNKNUMBER * CHUNKSIZE;
	  else localRy = (y + 1) * CHUNKSIZE - 1;

	  float delatx = (float)x / CHUNKNUMBER;
	  float delaty = (float)y / CHUNKNUMBER;

	  int level = (int)(texture(texHLevel, vec2(deltax,deltay)).r);
	  
	  int templevel = 6 - level;
	  if(grid < templevel) 
	  {
	   grid = templevel;
	   if(grid == 6)
	     break;
	  }

	  localLx = x * CHUNKSIZE + (int)((localLx - (int)((int)(x / pow(2.0, level)) * pow(2.0,level)) * CHUNKSIZE) / pow(2.0,level));
	  localRx = x * CHUNKSIZE +(int)((localRx - (int)((int)(x / pow(2.0, level)) * pow(2.0,level)) * CHUNKSIZE) / pow(2.0,level));
	  localLy = y * CHUNKSIZE +(int)((localLy - (int)((int)(y / pow(2.0, level)) * pow(2.0,level)) * CHUNKSIZE) / pow(2.0,level));
	  localRy = y * CHUNKSIZE +(int)((localRy - (int)((int)(y / pow(2.0, level)) * pow(2.0,level)) * CHUNKSIZE) / pow(2.0,level));

	  float yposbottom =  1.0 * localLy / float(CHUNKSIZE * CHUNKNUMBER);
	  float ypostop = 1.0 * localRy / float(CHUNKSIZE * CHUNKNUMBER);
	  float xposleft = 1.0 * localLx / float(CHUNKSIZE * CHUNKNUMBER);
	  float xposright = 1.0 * localRx / float(CHUNKSIZE * CHUNKNUMBER);

	  vec4 scaleBias = textureLod(vtex, vec2(xposleft,yposbottom), level);
	  vec2 pCoor = uv * scaleBias.x + scaleBias.zw;
	  vec4 sat = texture(Satmap, pCoor);
	  double lbret = double(sat.r / 256.0) + double(sat.g / 256.0 / 256.0) + double(sat.b / 256.0 / 256.0 / 256.0) + double(sat.a / 256.0 / 256.0 / 256.0 / 256.0);

	  scaleBias = textureLod(vtex, vec2(xposright,yposbottom), level);
	  pCoor = uv * scaleBias.x + scaleBias.zw;
	  sat = texture(Satmap, pCoor);
	  double rbret = double(sat.r / 256.0) + double(sat.g / 256.0 / 256.0) + double(sat.b / 256.0 / 256.0 / 256.0) + double(sat.a / 256.0 / 256.0 / 256.0 / 256.0);

	  scaleBias = textureLod(vtex, vec2(xposleft,ypostop), level);
	  pCoor = uv * scaleBias.x + scaleBias.zw;
	  sat = texture(Satmap, pCoor);
	  double ltret = double(sat.r / 256.0) + double(sat.g / 256.0 / 256.0) + double(sat.b / 256.0 / 256.0 / 256.0) + double(sat.a / 256.0 / 256.0 / 256.0 / 256.0);

	  scaleBias = textureLod(vtex, vec2(xposright,ypostop), level);
	  pCoor = uv * scaleBias.x + scaleBias.zw;
	  sat = texture(Satmap, pCoor);
	  double rtret = double(sat.r / 256.0) + double(sat.g / 256.0 / 256.0) + double(sat.b / 256.0 / 256.0 / 256.0) + double(sat.a / 256.0 / 256.0 / 256.0 / 256.0);

	  double ret = rtret - ltret - rbret + lbret;

	  ret = pow(ret, 1.0 / ep);
	  ret = ret * htop;
	  ret = ret / double(pow(2.0, level));

	  if(satvalue < ret) satvalue = ret;
	}

if(grid < 6)
{
	vec4 center = 0.25 * (Position_ES_in[0] + Position_ES_in[1] + Position_ES_in[2] + Position_ES_in[3]);
	float Dp = sqrt(center.x * center.x + center.z * center.z);
	vec4 maxside = Position_ES_in[2] - Position_ES_in[0];
	float s = sqrt(maxside.x * maxside.x + maxside.z * maxside.z) / 2.0;
	float d = 1 - s / Dp;
	if(d > 0)
	{
	  for(i = 0; i < 6 - grid;)
	  {
	    center.y = -currentPos.z;
	    center = center * d;
	    center.y += currentPos.z;
	    center.w = 1.0;
	    vec4 p = center;
	    p.y = center.y +  satvalue / 2;
	    center = gWVP * center;
		p = gWVP * center;
		float xPixel = (abs(center.x - p.x)) * WIDTH;
		float yPixel = (abs(center.y - p.y)) * HEIGHT;
		float flag = sqrt(xPixel * xPixel + yPixel * yPixel);
		if(flag < 2)
		{
		  break;
		}
		 grid = gird + 1;
		 satvalue = satvalue / 2.0;
	  }
	  grid = (int) pow(2.0,grid);
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

  outgrid[gl_InvocationID] = vec3(1.0 * grid / 64, 0, 0);

  gl_TessLevelOuter[0] = Position_ES_in[1].y;
  gl_TessLevelOuter[1] = Position_ES_in[2].y;
  gl_TessLevelOuter[2] = Position_ES_in[0].y;
  gl_TessLevelOuter[3] = Position_ES_in[3].y;
  gl_TessLevelInner[0] = grid;
  gl_TessLevelInner[1] = grid;
}