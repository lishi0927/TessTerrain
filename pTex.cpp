#include "pTex.h"

PTex::PTex(int pageSize, int physicalWidth) {
	this->payloadSize = pageSize;
	this->physicalWidth = physicalWidth;
}

void PTex::init(bool isHeightmap, int level, int virtualWidth) {
	if (!isHeightmap)
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
	if (isHeightmap) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R16, pixelSize, pixelSize,
			0, GL_RED, GL_UNSIGNED_SHORT, nullptr);
	}
	else {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGBA_S3TC_DXT1_EXT,
			pixelSize, pixelSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	Page *last = nullptr;
	for (int i = 0; i < physicalWidth; i++)
		for (int j = 0; j < physicalWidth; j++) {
			Page* t = new Page(isHeightmap);
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
		vector<Page*> tmp;
		tmp.clear();
		for (int j = 0; j < virtualWidth; j++)
			for (int k = 0; k < virtualWidth; k++) {
				tmp.push_back(nullptr);
			}
		pagePos.push_back(tmp);
		virtualWidth >>= 1;
		virtualWidth = (virtualWidth == 0) ? 1 : virtualWidth;
	}

	this->isHeightmap = isHeightmap;
}

void PTex::moveFront(Page* p) {
	if (p->prev)
		p->prev->next = p->next;
	if (p->next)
		p->next->prev = p->prev;

	if (p == tail)
		tail = tail->prev;

	if (p != head) {
		head->prev = p;
		p->next = head;
		head = p;
		head->prev = nullptr;
	}
}

void PTex::insert(int level, int x, int y, Page* p) {
	pagePos[level][y * (virtualWidth >> level) + x] = p;

	head->prev = p;
	p->next = head;
	head = p;
	head->prev = nullptr;
}

Page* PTex::getReplacePage() {
	count++;
	if (count > physicalWidth * physicalWidth) {
		printf("physical pages are not enough.");
		exit(0);
	}
	Page* p = tail;
	while (p->getLevel() == this->level - 1) {
		tail = tail->prev;
		moveFront(p);
		p = tail;
	}
	tail = tail->prev;
	tail->next = nullptr;
	int level = p->getLevel();
	if (level == -1)
		return p;
	int x = p->getVx();
	int y = p->getVy();
	pagePos[level][y * (virtualWidth >> level) + x] = nullptr;
	return p;
}

Page* PTex::findPage(int level, int x, int y) {
	return pagePos[level][y * (virtualWidth >> level) + x];
}

const int dx[9] = { -1, 1,  0, 0, 0, -1, -1,  1, 1 };
const int dy[9] = { 0, 0, -1, 1, 0, -1,  1, -1, 1 };

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

void PTex::update(int level, int x, int y, Page* tp) {
	
	int px = tp->getPx();
	int py = tp->getPy();

	if (isHeightmap) {

		glBindTexture(GL_TEXTURE_2D, tex);
		glTexSubImage2D(GL_TEXTURE_2D, 0, px * pageSize, py * pageSize,
			tp->getWidth(), tp->getHeight(),
			GL_RED, GL_UNSIGNED_SHORT, tp->getHeightmap());
		glBindTexture(GL_TEXTURE_2D, 0);
	}
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

void PTex::clearCount() {
	count = 0;
}