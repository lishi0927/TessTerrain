#include "page.h"


Page::Page(string dataname) {
	level = -1;
	prev = next = nullptr;
	data = nullptr;
	this->dataname = dataname;
	if (dataname[0] == 'd') {
		width = height = CHUNKSIZE + 2 * HBORDERSIZE;
		data = new ushort[width * height];
		memset(data, 0, width*height);
		cellByte = sizeof(ushort);
		borderSize = HBORDERSIZE;
		payloadSize = CHUNKSIZE;
	}
	else if (dataname[0] == 'm')
	{
		width = height = CHUNKSIZE + 2 * HBORDERSIZE;
		data = new double[width * height];
		memset(data, 0, width*height);
		cellByte = sizeof(double);
		borderSize = HBORDERSIZE;
		payloadSize = CHUNKSIZE;
	}
	else if (dataname[0] == 'A')
	{
		width = height = CHUNKSIZE + 2 * BORDERSIZE;
		data = new float[width * height];
		memset(data, 0, width*height);
		cellByte = sizeof(float);
		borderSize = BORDERSIZE;
		payloadSize = CHUNKSIZE;
	}
	else if (dataname[0] == 'L' || dataname[0] == 'n')
	{
		width = height = CHUNKSIZE + 2 * BORDERSIZE;
		data = new float[width * height * 3];
		memset(data, 0, sizeof(float) * width * height * 3);
		cellByte = sizeof(float) * 3;
		borderSize = BORDERSIZE;
		payloadSize = CHUNKSIZE;
	}
	else if (dataname[0] == 'V')
	{
		width = height = CHUNKSIZE + 2 * BORDERSIZE;
		data = new ushort[width * height * 4];
		memset(data, 0, width*height * 4);
		cellByte = sizeof(ushort) * 4;
		borderSize = BORDERSIZE;
		payloadSize = CHUNKSIZE;
	}
	else {
		width = height = BLENDSIZE + 2 * BORDERSIZE;
		data = new uchar[width * height * 3];
		memset(data, 0, width*height*3);
		cellByte = sizeof(uchar) * 3;
		borderSize = BORDERSIZE;
		payloadSize = BLENDSIZE;
	}
}

Page::~Page() {
	if (data)
		delete data;
}

void Page::loadData(int level, int x, int y) {
	if (dataname[0] == 'V') {
		loadVisPage(level, x, y);
	}
	else {
		loadPage(level, x, y);
	}
}

void Page::loadBorderData(int level, int tx, int ty, int pos) {
	int nSize = borderSize;
	int rp;
	rp = borderSize + (payloadSize >> level) * (1 << level);

	string filename;

	int offset;
	switch (dataname[0])
	{
	case 't':
		offset = 0;
		break;
	case 'm':
		offset = 20;
		break;
	default:
		offset = 12;
		break;
	}

	uchar *hp = nullptr;
	if (pos > CENTER) {
		switch (pos) {
		case LEFTTOP:
			filename = "data/" + getChunkName(tx << level, ty << level) + "/" + dataname;
			break;
		case RIGHTTOP:
			if ((tx << level) + (1 << level) - 1 >= CHUNKNUMBER)
				return;
			filename = "data/" + getChunkName((tx << level) + (1 << level) - 1, ty << level) + "/" + dataname;
			break;
		case LEFTBOTTOM:
			if ((ty << level) + (1 << level) - 1 >= CHUNKNUMBER)
				return;
			filename = "data/" + getChunkName(tx << level, (ty << level) + (1 << level) - 1) + "/" + dataname;
			break;
		case RIGHTBOTTOM:
			if ((tx << level) + (1 << level) - 1 >= CHUNKNUMBER || (ty << level) + (1 << level) - 1 >= CHUNKNUMBER)
				return;
			filename = "data/" + getChunkName((tx << level) + (1 << level) - 1, (ty << level) + (1 << level) - 1) + "/" + dataname;
			break;
		}
		filename += '0' + level;
		FILE *f;
		openfile(filename.c_str(), f);	
		int w, h;
		if (dataname[0] == 'd' || dataname[0] == 'A' || dataname[0] == 'm' || dataname[0] == 'L' || dataname[0] == 'n') {
			fread(&w, 4, 1, f);
			fread(&h, 4, 1, f);
			float tmp;
			fread(&tmp, sizeof(float), 1, f);
		}
		else {
			w = h = BLENDSIZE >> level;
		}

		for (int i = 0; i < nSize; i++) {
			switch (pos) {
			case LEFTTOP:
				seekfile(f, offset + i * w * cellByte, SEEK_SET);
				//hp = heightmap + (i + HBORDERSIZE + CHUNKSIZE + 1) * width
				//	- HBORDERSIZE;
				hp = (uchar*)data + ((i + rp) * width + rp) * cellByte;
				break;
			case RIGHTTOP:
				seekfile(f, offset + ((i + 1) * w - nSize) * cellByte, SEEK_SET);
				//hp = heightmap + (i + HBORDERSIZE + CHUNKSIZE) * width;
				hp = (uchar*)data + ((i + rp) * width) * cellByte;
				break;
			case LEFTBOTTOM:
				seekfile(f, offset + (h - nSize + i) * w * cellByte, SEEK_SET);
				//hp = heightmap + (i + 1) * width - HBORDERSIZE;
				hp = (uchar*)data + (i * width + rp) * cellByte;
				break;
			case RIGHTBOTTOM:
				seekfile(f, offset + ((h - nSize + i + 1) * w - nSize) * cellByte,
					SEEK_SET);
				hp = (uchar*)data + i * width * cellByte;
				break;
			}
			fread(hp, nSize * cellByte, 1, f);
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
				filename = "data/" + getChunkName(x, y) + "/" + dataname;
				filename += '0' + level;
				FILE *f;
				openfile(filename.c_str(), f);
				int w, h;
				if (dataname[0] == 'd' || dataname[0] == 'A' || dataname[0] == 'm' || dataname[0] == 'L' || dataname[0] == 'n') {
					fread(&w, 4, 1, f);
					fread(&h, 4, 1, f);
					float tmp;
					fread(&tmp, sizeof(float), 1, f);
				}
				else {
					w = h = BLENDSIZE >> level;
				}
				assert(h >= nSize);
				for (int i = 0; i < nSize; i++) {
					if (pos == TOP) {
						//hp = heightmap + (i + HBORDERSIZE + CHUNKSIZE) * width +
							//HBORDERSIZE + (x - sX) * w;
						hp = (uchar*)data + ((i + rp) * width +	borderSize + (x - sX) * w) * cellByte;
						seekfile(f, offset + i * w * cellByte, SEEK_SET);
						fread(hp, w * cellByte, 1, f);
					} 
					else {
						hp = (uchar*) data + (i * width + borderSize + (x - sX) * w) * cellByte;
						seekfile(f, offset + (h - nSize + i) * w * cellByte, SEEK_SET);
						fread(hp, w * cellByte, 1, f);
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
				x = (tx << level) + (1 << level) -1;
			for (int y = sY; y < eY; y++) {
				filename = "data/" + getChunkName(x, y) + "/" + dataname;
				filename += '0' + level;
				FILE *f;
				openfile(filename.c_str(), f);
				int w, h;
				if (dataname[0] == 'd' || dataname[0] == 'A' || dataname[0] == 'm' || dataname[0] == 'L' || dataname[0] == 'n') {
					fread(&w, 4, 1, f);
					fread(&h, 4, 1, f);
					assert(w >= nSize);
					float tmp;
					fread(&tmp, sizeof(float), 1, f);
				}
				else
					w = h = BLENDSIZE >> level;
				for (int i = 0; i < h; i++) {
					if (pos == LEFT) {
						hp = (uchar*)data + (((y - sY) * h + i + borderSize) * width + rp) * cellByte;
						seekfile(f, offset + i * w * cellByte, SEEK_SET);
						fread(hp, nSize * cellByte, 1, f);
					}
					else {
						hp = (uchar*)data + ((y - sY) * h + i + borderSize) * width * cellByte;
						seekfile(f, offset + ((i + 1) * w - nSize) * cellByte, SEEK_SET);
						fread(hp, nSize * cellByte, 1, f);
					}
				}
				fclose(f);
			}
		}
	}
}

void Page::loadmap(int level, int tx, int ty, int pos) {
	if (pos != CENTER) {
		loadBorderData(level, tx, ty, pos);
		return;
	}

	int sX, eX, sY, eY;
	sX = tx << level;
	eX = (tx << level) + (1 << level);
	clamp(eX, 0, CHUNKNUMBER);
	sY = ty << level;
	eY = (ty << level) + (1 << level);
	clamp(eY, 0, CHUNKNUMBER);

	uchar *hp = nullptr;
	for (int y = sY; y < eY; y++) 
		for (int x = sX; x < eX; x++) {
			string filename = "data/" + getChunkName(x, y) + "/" + dataname;
			filename += '0' + level;
			FILE *f;
			openfile(filename.c_str(), f);

			int w, h;
			if (dataname[0] == 'd' || dataname[0] == 'A' || dataname[0] == 'L' || dataname[0] == 'n') {
				fread(&w, 4, 1, f);
				fread(&h, 4, 1, f);
				float tmp;
				fread(&tmp, sizeof(float), 1, f);
			}
			else if (dataname[0] == 'm')
			{
				fread(&w, 4, 1, f);
				fread(&h, 4, 1, f);
				int htop, hoff, ep;
				fread(&htop, sizeof(int), 1, f);
				fread(&hoff, sizeof(int), 1, f);
				fread(&ep, sizeof(int), 1, f);
			}
			else
				w = h = BLENDSIZE >> level;

			for (int i = 0; i < h; i++) {
				hp = (uchar*)data + (((y - sY) * h + borderSize + i) * width +
					((x - sX) * w + borderSize)) * cellByte;
				fread(hp, w * cellByte, 1, f);
			}
			fclose(f);
		}
}

const int dx[9] = { -1, 1,  0, 0, 0, -1, -1,  1, 1 };
const int dy[9] = {  0, 0, -1, 1, 0, -1,  1, -1, 1 };

void Page::loadPage(int level, int x, int y) {	
	int vw, vh;
	vw = CHUNKNUMBER >> level;
	vw = (vw == 0) ? 1 : vw;
	vh = vw;

	int tx, ty;
	for (int i = 0; i < 9; i++) {
		tx = x + dx[i];
		ty = y + dy[i];

		if (tx < 0 || ty < 0 || tx >= vw || ty >= vh)
			continue;
		loadmap(level, tx, ty, i);
	}
}

void Page::loadVisPage(int level, int x, int y) {
	int vw, vh;
	vw = CHUNKNUMBER >> ((level-1) >= 0 ? (level-1):0);
	vw = (vw == 0) ? 1 : vw;
	vh = vw;

	int tx, ty;
	for (int i = 0; i < 9; i++) {
		tx = x + dx[i];
		ty = y + dy[i];

		if (tx < 0 || ty < 0 || tx >= vw || ty >= vh)
			continue;
		loadVismap(level, tx, ty, i);
	}
}

void Page::loadVismap(int level, int tx, int ty, int pos)
{
	if (pos != CENTER) {
		loadVisBorderData(level, tx, ty, pos);
		return;
	}

	int sX, eX, sY, eY;
	int reallevel = (level - 1) >= 0 ? (level - 1) : 0;
	sX = tx << reallevel;
	eX = (tx << reallevel) + (1 << reallevel);
	clamp(eX, 0, CHUNKNUMBER);
	sY = ty << reallevel;
	eY = (ty << reallevel) + (1 << reallevel);
	clamp(eY, 0, CHUNKNUMBER);

	uchar *hp = nullptr;
	for (int y = sY; y < eY; y++)
		for (int x = sX; x < eX; x++) {
			string filename = "data/" + getChunkName(x, y) + "/" + dataname;
			filename += '0' + level;
			FILE *f;
			openfile(filename.c_str(), f);

			int w, h;			
			fread(&w, 4, 1, f);
			fread(&h, 4, 1, f);		

			for (int i = 0; i < h; i++) {
				hp = (uchar*)data + (((y - sY) * h + borderSize + i) * width +
					((x - sX) * w + borderSize)) * cellByte;
				fread(hp, w * cellByte, 1, f);
			}
			fclose(f);
		}
}

void Page::loadVisBorderData(int level, int tx, int ty, int pos)
{
	int nSize = borderSize;


	string filename;

	int offset;
	
	offset = 8;

	int reallevel = (level - 1) >= 0 ? (level - 1) : 0;

	int rp;
	rp = borderSize + (payloadSize >> reallevel) * (1 << reallevel);

	uchar *hp = nullptr;
	if (pos > CENTER) {
		switch (pos) {
		case LEFTTOP:
			filename = "data/" + getChunkName(tx << reallevel, ty << reallevel) + "/" + dataname;
			break;
		case RIGHTTOP:
			if ((tx << reallevel) + (1 << reallevel) - 1 >= CHUNKNUMBER)
				return;
			filename = "data/" + getChunkName((tx << reallevel) + (1 << reallevel) - 1, ty << reallevel) + "/" + dataname;
			break;
		case LEFTBOTTOM:
			if ((ty << reallevel) + (1 << reallevel) - 1 >= CHUNKNUMBER)
				return;
			filename = "data/" + getChunkName(tx << reallevel, (ty << reallevel) + (1 << reallevel) - 1) + "/" + dataname;
			break;
		case RIGHTBOTTOM:
			if ((tx << reallevel) + (1 << reallevel) - 1 >= CHUNKNUMBER || (ty << reallevel) + (1 << reallevel) - 1 >= CHUNKNUMBER)
				return;
			filename = "data/" + getChunkName((tx << reallevel) + (1 << reallevel) - 1, (ty << reallevel) + (1 << reallevel) - 1) + "/" + dataname;
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
				seekfile(f, offset + i * w * cellByte, SEEK_SET);
				hp = (uchar*)data + ((i + rp) * width + rp) * cellByte;
				break;
			case RIGHTTOP:
				seekfile(f, offset + ((i + 1) * w - nSize) * cellByte, SEEK_SET);
				hp = (uchar*)data + ((i + rp) * width) * cellByte;
				break;
			case LEFTBOTTOM:
				seekfile(f, offset + (h - nSize + i) * w * cellByte, SEEK_SET);
				hp = (uchar*)data + (i * width + rp) * cellByte;
				break;
			case RIGHTBOTTOM:
				seekfile(f, offset + ((h - nSize + i + 1) * w - nSize) * cellByte,
					SEEK_SET);
				hp = (uchar*)data + i * width * cellByte;
				break;
			}
			fread(hp, nSize * cellByte, 1, f);
		}
		fclose(f);
	}
	else {
		if (pos == TOP || pos == BOTTOM) {
			int sX = tx << reallevel;
			int eX = sX + (1 << reallevel);
			int y;
			if (pos == TOP)
				y = ty << reallevel;
			else
				y = (ty << reallevel) + (1 << reallevel) - 1;
			for (int x = sX; x < eX; x++) {
				filename = "data/" + getChunkName(x, y) + "/" + dataname;
				filename += '0' + level;
				FILE *f;
				openfile(filename.c_str(), f);
				int w, h;
				fread(&w, 4, 1, f);
				fread(&h, 4, 1, f);
				assert(h >= nSize);
				for (int i = 0; i < nSize; i++) {
					if (pos == TOP) {
						hp = (uchar*)data + ((i + rp) * width + borderSize + (x - sX) * w) * cellByte;
						seekfile(f, offset + i * w * cellByte, SEEK_SET);
						fread(hp, w * cellByte, 1, f);
					}
					else {
						hp = (uchar*)data + (i * width + borderSize + (x - sX) * w) * cellByte;
						seekfile(f, offset + (h - nSize + i) * w * cellByte, SEEK_SET);
						fread(hp, w * cellByte, 1, f);
					}
				}
				fclose(f);
			}
		}
		else {
			int sY = ty << reallevel;
			int eY = sY + (1 << reallevel);
			int x;
			if (pos == LEFT)
				x = tx << reallevel;
			else
				x = (tx << reallevel) + (1 << reallevel) - 1;
			for (int y = sY; y < eY; y++) {
				filename = "data/" + getChunkName(x, y) + "/" + dataname;
				filename += '0' + level;
				FILE *f;
				openfile(filename.c_str(), f);
				int w, h;
				fread(&w, 4, 1, f);
				fread(&h, 4, 1, f);
				for (int i = 0; i < h; i++) {
					if (pos == LEFT) {
						hp = (uchar*)data + (((y - sY) * h + i + borderSize) * width + rp) * cellByte;
						seekfile(f, offset + i * w * cellByte, SEEK_SET);
						fread(hp, nSize * cellByte, 1, f);
					}
					else {
						hp = (uchar*)data + ((y - sY) * h + i + borderSize) * width * cellByte;
						seekfile(f, offset + ((i + 1) * w - nSize) * cellByte, SEEK_SET);
						fread(hp, nSize * cellByte, 1, f);
					}
				}
				fclose(f);
			}
		}
	}
}