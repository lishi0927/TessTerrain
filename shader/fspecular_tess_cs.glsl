#version 410

layout(vertices = 4) out;


// attributes of the input CPs                                                                  
in vec3 Position_CS_in[];

// attributes of the output CPs                                                                 
out vec3 Position_ES_in[];

uniform sampler2D texHLevel;

uniform vec4 currentPos;

const float VIEWCHUNKNUMBER = 16.0;
const float CHUNKNUMBER = 32.0;
const float CHUNKSIZE = 512.0;
const float MAXSCALE =  10 * CHUNKSIZE * VIEWCHUNKNUMBER / 4.0f;

const float WIDTH = 1024;
const float HEIGHT = 720;


void main()
{
  // Set the control points of the output patch                                               
   Position_ES_in[gl_InvocationID] = Position_CS_in[gl_InvocationID];
   
	float lx = Position_ES_in[0].x/ MAXSCALE / 4.0 * VIEWCHUNKNUMBER / CHUNKNUMBER + currentPos.x;
	float rx = Position_ES_in[1].x/ MAXSCALE / 4.0 * VIEWCHUNKNUMBER / CHUNKNUMBER + currentPos.x;
	float ly = Position_ES_in[0].z/ MAXSCALE / 4.0 * VIEWCHUNKNUMBER / CHUNKNUMBER + currentPos.y;
	float ry = Position_ES_in[2].z/ MAXSCALE / 4.0 * VIEWCHUNKNUMBER / CHUNKNUMBER + currentPos.y;
 
    lx = clamp(lx, 0.0, 1.0);
    ly = clamp(ly, 0.0, 1.0);
    rx = clamp(rx, 0.0, 1.0);
    ry = clamp(ry, 0.0, 1.0);

    int blx, bly, brx, bry;
    blx = int(floor(lx * CHUNKNUMBER));
    bly = int(floor(ly * CHUNKNUMBER));
    brx = int(floor(rx * CHUNKNUMBER));
    bry = int(floor(ry * CHUNKNUMBER));
 
    int grid = 0;

    for (int x = blx; x <= brx; x++)
      for (int y = bly; y <= bry; y++) {

		float deltax = x / CHUNKNUMBER;
		float deltay = y / CHUNKNUMBER;

		int level = int(texture(texHLevel, vec2(deltax,deltay)).r);

		int templevel = 6 - level;
		if(grid < templevel) 
		{
		   grid = templevel;
		}
	  }
    
	grid = int(pow(2.0,grid));

    gl_TessLevelOuter[0] = Position_ES_in[1].y;
    gl_TessLevelOuter[1] = Position_ES_in[2].y;
    gl_TessLevelOuter[2] = Position_ES_in[0].y;
    gl_TessLevelOuter[3] = Position_ES_in[3].y;
    gl_TessLevelInner[0] = grid;
    gl_TessLevelInner[1] = grid;
}