#ifndef PAGE_H
#define PAGE_H

#include "util.h"

class Page {
private:
	int level, vx, vy;
	int px, py;

	bool isHeightmap;

	int width, height;
	ushort *heightmap;
	uchar *pixels;

	void loadBorderData(int level, int tx, int ty, int pos);
	void loadHeightmap(int level, int tx, int ty, int pos);
	void loadHeightPage(int level, int x, int y);

public:
	Page *prev, *next;

	Page(bool isHeightmap);
	~Page();
	void free();
	int getLevel() const;
	int getVx() const;
	int getVy() const;
	int getPx() const;
	int getPy() const;
	void setVirtualPos(int level, int x, int y);
	void setPhysicalPos(int x, int y);
	int getWidth() const;
	int getHeight() const;
	ushort * getHeightmap();
	uchar * getPixels();
	void loadPage(int level, int x, int y);
};
#endif