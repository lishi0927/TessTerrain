#ifndef UTILS_H
#define UTILS_H

#include <vector>
#include <cassert>
#include <string>
#include <glm/glm.hpp>
#include <GL/glew.h>
#include <windows.h>
#include <process.h>
#include <set>

#define CHUNKNUMBER (32)
#define CHUNKSIZE (512)
#define VIEWCHUNKNUMBER (16)
#define LEVELOFTERRAIN (7)

#define CHUNKREALSIZE (5120)

#define BLENDSIZE (512)
#define LEVELOFBLENDTEX (7)

#define XMIN (15)
#define YMIN (-24)

#define LEVEL (6)
#define FINER (1)

#define Min(a, b) ((a) < (b) ? (a) : (b))
#define Max(a, b) ((a) > (b) ? (a) : (b))

#define RIGHT (0)
#define LEFT (1)
#define BOTTOM (2)
#define TOP (3)
#define CENTER (4)
#define RIGHTBOTTOM (5)
#define RIGHTTOP (6)
#define LEFTBOTTOM (7)
#define LEFTTOP (8)

#define BORDERSIZE (4)
#define HBORDERSIZE (4)

#define M_PI       3.14159265358979323846   // pi

#define sqr(x) ((x) * (x))

typedef unsigned short ushort;
typedef unsigned char  uchar;

#define TERRAINFILE "dem"

const int GRID = CHUNKSIZE;
//const int GRID = CHUNKSIZE;
const float MAXSCALE = 200.0f * VIEWCHUNKNUMBER / 4.0f;
const int WIDTH = 1024, HEIGHT = 720;
const int FEEDBACK_WIDTH = WIDTH / 10;
const int FEEDBACK_HEIGHT = HEIGHT / 10;
const float fov = 45.0f;
const float znear = 0.1f;
const float zfar = 2000.0f;

using namespace std;

string getChunkName(int x, int y);

void clamp(int &x, int l, int r);

float clampf(float x, float a, float b);

void openfile(const char* filename, FILE* &fp);

void seekfile(FILE* fp, int off, int pos);
#endif