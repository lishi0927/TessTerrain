#include "page.h"

Page::Page(bool isHeightmap) {
	level = -1;
	prev = next = nullptr;
	this->isHeightmap = isHeightmap;
	heightmap = nullptr;
	pixels = nullptr;
	if (isHeightmap) {
		width = height = CHUNKSIZE + 2 * HBORDERSIZE;
		heightmap = new ushort[width * height];
	}
	else {
		width = height = BLENDSIZE + 2 * BORDERSIZE;
		pixels = new uchar[width * height];
	}
}

Page::~Page() {
	free();
}

void Page::free() {
	if (heightmap)
		delete heightmap;
	if (pixels)
		delete pixels;
}

int Page::getLevel() const {
	return level;
}

int Page::getVx() const {
	return vx;
}

int Page::getVy() const {
	return vy;
}

int Page::getPx() const {
	return px;
}

int Page::getPy() const {
	return py;
}

void Page::setVirtualPos(int level, int x, int y) {
	this->level = level;
	this->vx = x;
	this->vy = y;
}

void Page::setPhysicalPos(int x, int y) {
	this->px = x;
	this->py = y;
}

int Page::getWidth() const {
	return width;
}

int Page::getHeight() const {
	return height;
}

ushort* Page::getHeightmap() {
	return heightmap;
}

uchar* Page::getPixels() {
	return pixels;
}

void Page::loadBorderData(int level, int tx, int ty, int pos) {
	int nSize = HBORDERSIZE;
	int rp;
	rp = HBORDERSIZE + (CHUNKSIZE >> level) * (1 << level);

	string filename;

	int offset = 12;

	unsigned short *hp = nullptr;
	if (pos > CENTER) {
		switch (pos) {
		case LEFTTOP:
			filename = "data/" + getChunkName(tx << level, ty << level) + "/" + TERRAINFILE;
			break;
		case RIGHTTOP:
			if ((tx << level) + (1 << level) - 1 >= CHUNKNUMBER)
				return;
			filename = "data/" + getChunkName((tx << level) + (1 << level) - 1, ty << level) + "/" + TERRAINFILE;
			break;
		case LEFTBOTTOM:
			if ((ty << level) + (1 << level) - 1 >= CHUNKNUMBER)
				return;
			filename = "data/" + getChunkName(tx << level, (ty << level) + (1 << level) - 1) + "/" + TERRAINFILE;
			break;
		case RIGHTBOTTOM:
			if ((tx << level) + (1 << level) - 1 >= CHUNKNUMBER || (ty << level) + (1 << level) - 1 >= CHUNKNUMBER)
				return;
			filename = "data/" + getChunkName((tx << level) + (1 << level) - 1, (ty << level) + (1 << level) - 1) + "/" + TERRAINFILE;
			break;
		}
		filename += '0' + level;
		FILE *f;
		openfile(filename.c_str(), f);
		int w, h;
		fread(&w, 4, 1, f);
		fread(&h, 4, 1, f);

		for (int i = 0; i < nSize; i++) {
			switch (pos) {
			case LEFTTOP:
				seekfile(f, offset + i * w * sizeof(short), SEEK_SET);
				hp = heightmap + (i + rp) * width + rp;
				break;
			case RIGHTTOP:
				seekfile(f, offset + ((i + 1) * w - nSize) * sizeof(short), SEEK_SET);
				hp = heightmap + (i + rp) * width;
				break;
			case LEFTBOTTOM:
				seekfile(f, offset + (h - nSize + i) * w * sizeof(short), SEEK_SET);
				hp = heightmap + i * width + rp;
				break;
			case RIGHTBOTTOM:
				seekfile(f, offset + ((h - nSize + i + 1) * w - nSize) * sizeof(short),
					SEEK_SET);
				hp = heightmap + i * width;
				break;
			}
			fread(hp, nSize * sizeof(short), 1, f);
		}
		fclose(f);
	}
	else {
		if (pos == TOP || pos == BOTTOM) {
			int sX = tx << level;
			int eX = sX + (1 << level);
			int y;
			if (pos == TOP)
				y = ty << level;
			else
				y = (ty << level) + (1 << level) - 1;
			for (int x = sX; x < eX; x++) {
				filename = "data/" + getChunkName(x, y) + "/" + TERRAINFILE;
				filename += '0' + level;
				FILE *f;
				openfile(filename.c_str(), f);
				int w, h;
				fread(&w, 4, 1, f);
				fread(&h, 4, 1, f);
				assert(h >= nSize);
				for (int i = 0; i < nSize; i++) {
					if (pos == TOP) {
						hp = heightmap + (i + rp) * width + HBORDERSIZE + (x - sX) * w;
						seekfile(f, offset + i * w * sizeof(short), SEEK_SET);
						fread(hp, w * sizeof(short), 1, f);
					}
					else {
						hp = heightmap + i * width + HBORDERSIZE + (x - sX) * w;
						seekfile(f, offset + (h - nSize + i) * w * sizeof(short), SEEK_SET);
						fread(hp, w * sizeof(short), 1, f);
					}
				}
				fclose(f);
			}
		}
		else {
			int sY = ty << level;
			int eY = sY + (1 << level);
			int x;
			if (pos == LEFT)
				x = tx << level;
			else
				x = (tx << level) + (1 << level) - 1;
			for (int y = sY; y < eY; y++) {
				filename = "data/" + getChunkName(x, y) + "/" + TERRAINFILE;
				filename += '0' + level;
				FILE *f;
				openfile(filename.c_str(), f);
				int w, h;
				fread(&w, 4, 1, f);
				fread(&h, 4, 1, f);
				assert(w >= nSize);
				for (int i = 0; i < h; i++) {
					if (pos == LEFT) {
						hp = heightmap + ((y - sY) * h + i + HBORDERSIZE) * width + rp;
						seekfile(f, offset + i * w * sizeof(short), SEEK_SET);
						fread(hp, nSize * sizeof(short), 1, f);
					}
					else {
						hp = heightmap + ((y - sY) * h + i + HBORDERSIZE) * width;
						seekfile(f, offset + ((i + 1) * w - nSize) * sizeof(short), SEEK_SET);
						fread(hp, nSize * sizeof(short), 1, f);
					}
				}
				fclose(f);
			}
		}
	}
}

void Page::loadHeightmap(int level, int tx, int ty, int pos) {
	if (pos != CENTER) {
		loadBorderData(level, tx, ty, pos);
		return;
	}

	int sX, eX, sY, eY;
	sX = tx << level;
	eX = (tx << level) + (1 << level);
	clamp(eX, 0, CHUNKNUMBER - 1);
	sY = ty << level;
	eY = (ty << level) + (1 << level);
	clamp(eY, 0, CHUNKNUMBER - 1);

	unsigned short *hp = nullptr;
	for (int y = sY; y < eY; y++)
		for (int x = sX; x < eX; x++) {
			string filename = "data/" + getChunkName(x, y) + "/" + TERRAINFILE;
			filename += '0' + level;
			FILE *f;
			openfile(filename.c_str(), f);

			int w, h;
			fread(&w, 4, 1, f);
			fread(&h, 4, 1, f);

			float tmp;
			fread(&tmp, sizeof(float), 1, f);

			for (int i = 0; i < h; i++) {
				hp = heightmap + ((y - sY) * h + HBORDERSIZE + i) * width +
					((x - sX) * w + HBORDERSIZE);
				fread(hp, w * sizeof(short), 1, f);
			}
			fclose(f);
		}
}

const int dx[9] = { -1, 1,  0, 0, 0, -1, -1,  1, 1 };
const int dy[9] = { 0, 0, -1, 1, 0, -1,  1, -1, 1 };

void Page::loadHeightPage(int level, int x, int y) {
	int vw, vh;
	vw = CHUNKNUMBER >> level;
	if (vw == 0)
		vw = 1;
	vh = vw;

	int tx, ty;
	for (int i = 0; i < 9; i++) {
		tx = x + dx[i];
		ty = y + dy[i];

		if (tx < 0 || ty < 0 || tx >= vw || ty >= vh)
			continue;
		loadHeightmap(level, tx, ty, i);
	}
}
void Page::loadPage(int level, int x, int y) {
	if (isHeightmap) {
		loadHeightPage(level, x, y);
	}
}