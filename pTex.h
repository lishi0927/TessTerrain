#ifndef PTEX_H
#define PTEX_H

#include "util.h"
#include "page.h"
#include <map>
#include <list>
#include "terrainData.h"
#include "ddsloader.h"
#include "sat.h"
using namespace std;

class PTex {
private:
	int pageSize, physicalWidth, pixelSize;
	int level, virtualWidth;
	int borderSize, payloadSize;
	bool isHeightmap;
	GLuint tex;
	Page *head, *tail;
	vector<vector<Page *> >pagePos;
	int count;

public:
	PTex(int pageSize, int physicalWidth);
	void init(bool isHeightmap, int level, int virtualWidth);
	void moveFront(Page * p);
	void insert(int level, int x, int y, Page * p);
	Page * getReplacePage();
	Page * findPage(int level, int x, int y);
	void calc_para(int i, int & xoff, int & yoff, int px, int py);
	void update(int level, int x, int y, Page* tp);
	int getPixelSize() const;
	int getPhysicalWidth() const;
	int getPayloadSize() const;
	int getBorderSize() const;
	GLuint getTex();
	void clearCount();
};

class PageInfo {
	int level, x, y;
};


#endif
