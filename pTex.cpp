#include "pTex.h"
#include "util.h"
#include <iostream>
#include <bitset>

PTex::PTex(int pageSize, int physicalWidth, string dataname) {
	this->payloadSize = pageSize;
	this->physicalWidth = physicalWidth;

	maxSize = physicalWidth * physicalWidth;
	this->dataname = dataname;
}

void PTex::init(int level, int virtualWidth) {
	if (dataname[0] != 'd' && dataname[0] != 'm')
		this->borderSize = BORDERSIZE;
 	else
 		this->borderSize = HBORDERSIZE;
	this->pageSize = payloadSize + 2 * this->borderSize;
	this->pixelSize = this->pageSize * this->physicalWidth;

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	if (dataname[0] == 'd') {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, pixelSize, pixelSize,
			0, GL_RED, GL_UNSIGNED_SHORT, nullptr);
	}
	else if (dataname[0] == 'm') {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, pixelSize, pixelSize,
			0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
	}
	else if (dataname[0] == 'A') {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, pixelSize, pixelSize,
			0, GL_RED, GL_FLOAT, nullptr);
	}
	else if (dataname[0] == 'L' || dataname[0] == 'n') {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pixelSize, pixelSize, 0, GL_RGB, GL_FLOAT, nullptr);
	}
	else if (dataname[0] == 'V')
	{
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16, pixelSize, pixelSize,
			0, GL_RGBA, GL_UNSIGNED_SHORT, nullptr);
	}
	else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, pixelSize, pixelSize, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	BasePage *last = nullptr;
	for (int i = 0; i < physicalWidth; i++) 
		for (int j = 0; j < physicalWidth; j++) {
			BasePage* t = new Page(dataname);
			t->setPhysicalPos(j, i);
			if (!i && !j)
				head = t;
			if (i == physicalWidth - 1 && j == i)
				tail = t;
			if (last)
				last->next = t;
			t->prev = last;
			last = t;
		}

	this->level = level;
	this->virtualWidth = virtualWidth;
	for (int i = 0; i < level; i++) {
		vector<BasePage*> tmp;
		tmp.clear();
		for (int j = 0; j < virtualWidth; j++)
			for (int k = 0; k < virtualWidth; k++) {
				tmp.push_back(nullptr);
			}
		pagePos.push_back(tmp);
		virtualWidth >>= 1;
		virtualWidth = (virtualWidth == 0) ? 1 : virtualWidth;
	}
}

const int dx[9] = { -1, 1,  0, 0, 0, -1, -1,  1, 1 };
const int dy[9] = {  0, 0, -1, 1, 0, -1,  1, -1, 1 };

void PTex::calc_para(int i, int &xoff, int &yoff, int px, int py) {
	switch (i) {
	case TOP:
		xoff = px * pageSize + borderSize;
		yoff = (py + 1) * pageSize - borderSize;
		break;
	case BOTTOM:
		xoff = px * pageSize + borderSize;
		yoff = py * pageSize;
		break;
	case LEFT:
		xoff = (px + 1) * pageSize - borderSize;
		yoff = py * pageSize + borderSize;
		break;
	case RIGHT:
		xoff = px * pageSize;
		yoff = py * pageSize + borderSize;
		break;
	case CENTER:
		xoff = px * pageSize + borderSize;
		yoff = py * pageSize + borderSize;
		break;
	case LEFTTOP:
		xoff = (px + 1) * pageSize - borderSize;
		yoff = (py + 1) * pageSize - borderSize;
		break;
	case RIGHTTOP:
		xoff = px * pageSize;
		yoff = (py + 1) * pageSize - borderSize;
		break;
	case LEFTBOTTOM:
		xoff = (px + 1) * pageSize - borderSize;
		yoff = py * pageSize;
		break;
	case RIGHTBOTTOM:
		xoff = px * pageSize;
		yoff = py * pageSize;
		break;
	}
}

void PTex::update(int level, int x, int y, BasePage* tp) {
	//string filename;
	//static TerrainData terrainData;
	//static DDSLoader ddsloader;

	int px = tp->getPx();
	int py = tp->getPy();

	glBindTexture(GL_TEXTURE_2D, tex);
	if (dataname[0] == 'd') {
		glTexSubImage2D(GL_TEXTURE_2D, 0, px * pageSize, py * pageSize,
			tp->getWidth(), tp->getHeight(),
			GL_RED, GL_UNSIGNED_SHORT, (ushort*)tp->getData());
	}
	else if (dataname[0] == 'm') {
		int twidth = tp->getWidth();
		int theight = tp->getHeight();
		unsigned short* newdata = new unsigned short[twidth * theight * 4];
		for(int i = 0; i < theight; i++)
			for (int j = 0; j < twidth; j++)
			{
				double result = ((double*)(tp->getData()))[i * twidth + j];
				result = 100;
				uint64_t representation = getRepresentation(result);
				for (int k = 0; k < 4; k++)
				{
					ushort result1 = representation & 0xFFFF;
					representation = representation >> 16;
					newdata[(i* twidth + j) * 4 + k] = result1;
				}
			}
		glTexSubImage2D(GL_TEXTURE_2D, 0, px * pageSize, py * pageSize,
			twidth, theight, GL_RGBA, GL_UNSIGNED_SHORT, newdata);
		delete []newdata;
	}
	else if (dataname[0] == 'A') {
		glTexSubImage2D(GL_TEXTURE_2D, 0, px * pageSize, py * pageSize,
			tp->getWidth(), tp->getHeight(), GL_RED, GL_FLOAT, (float*)tp->getData());
	}
	else if (dataname[0] == 'L' || dataname[0] == 'n')
	{
	glTexSubImage2D(GL_TEXTURE_2D, 0, px * pageSize, py * pageSize,
	    tp->getWidth(), tp->getHeight(), GL_RGB, GL_FLOAT, (float*)tp->getData());
	}
	else if (dataname[0] == 'V')
	{
	    glTexSubImage2D(GL_TEXTURE_2D, 0, px * pageSize, py * pageSize,
			tp->getWidth(), tp->getHeight(), GL_RGBA, GL_UNSIGNED_SHORT, (ushort*)tp->getData());
	}
	else {
		glTexSubImage2D(GL_TEXTURE_2D, 0, px * pageSize, py * pageSize,
			tp->getWidth(), tp->getHeight(), GL_RGB, GL_UNSIGNED_BYTE, (uchar*)tp->getData());
	}
	glBindTexture(GL_TEXTURE_2D, 0);
}

int PTex::getPixelSize() const {
	return pixelSize;
}

int PTex::getPhysicalWidth() const {
	return physicalWidth;
}

int PTex::getPayloadSize() const {
	return payloadSize;
}

int PTex::getBorderSize() const {
	return borderSize;
}

GLuint PTex::getTex() {
	return tex;
}
