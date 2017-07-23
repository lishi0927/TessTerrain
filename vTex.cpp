#include "vTex.h"

VTex::VTex() {
}

VTex::~VTex() {
	delete ptex;
}

void VTex::init(int pageSize, int virtualWidth, bool isHeightmap, int maxLevel) {
	this->pageSize = pageSize;
	this->virtualWidth = virtualWidth;

	ptable.clear();
	w.clear();
	h.clear();

	length.clear();

	int tmp;
	for (int i = 0; i < maxLevel; i++) {
		length.push_back((pageSize >> i) * virtualWidth);
		tmp = virtualWidth >> i;
		tmp = (tmp <= 0) ? 1 : tmp;
		w.push_back(tmp);
		h.push_back(tmp);
		ptable.push_back(new float[w[i] * h[i] * 4]);
		memset(ptable.back(), 0, w[i] * h[i] * 4 * 4);

	}

	this->maxLevel = maxLevel;
	this->isHeightmap = isHeightmap;

	if (isHeightmap) {

		ptex = new PTex(pageSize, VIEWCHUNKNUMBER * 1.5);
		ptex->init(true, maxLevel, virtualWidth);
	}

	else {
		ptex = new PTex(pageSize, VIEWCHUNKNUMBER * 1.5);
		ptex->init(false, maxLevel, virtualWidth);
	}


	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);

	glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, maxLevel - 1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, w[0], h[0], 0,
		GL_RGBA, GL_FLOAT, nullptr);

	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	loadingSet.clear();
	handleList.clear();
}

HANDLE VTex::event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
CRITICAL_SECTION VTex::criticalSection;

void VTex::update(int k, int x, int y) {
	if (loadingSet.find(make_pair(k, y * w[k] + x)) != loadingSet.end())
		return;


	Page *tp = ptex->findPage(k, x, y);

	if (tp != nullptr) {
		ptex->moveFront(tp);
		return;
	}
	else {
		tp = ptex->getReplacePage();
		unmap(tp);

		UpdateInfo uInfo;
		ThreadInfo tInfo;
		uInfo.level = k;
		uInfo.x = x;
		uInfo.y = y;
		uInfo.page = tInfo.page = tp;
		uInfo.vtex = this;
		uInfo.ptex = ptex;
		loadingSet.insert(make_pair(k, y * w[k] + x));
		tInfo.handle = (HANDLE)_beginthreadex(nullptr, 0, updatePage, &uInfo, 0, nullptr);
		assert(tInfo.handle != 0);
		handleList.push_back(tInfo);
		WaitForSingleObject(event, INFINITE);

	}
}


void VTex::update(float currentX, float currentY, float currentH, float hLevel[], unsigned char hLevel1[],
	glm::mat4 matrix) {
	static float eList[CHUNKNUMBER][CHUNKNUMBER][LEVELOFTERRAIN];
	static float H[CHUNKNUMBER][CHUNKNUMBER];
	static bool clear = false;
	if (!clear) {
		for (int i = 0; i < CHUNKNUMBER; i++)
			for (int j = 0; j < CHUNKNUMBER; j++) {
				eList[i][j][0] = -1;
			}
		clear = true;
	}
	float lx, rx, ly, ry;
	float r = VIEWCHUNKNUMBER;
	r /= CHUNKNUMBER;
	lx = currentX - r / 2;
	rx = currentX + r / 2;
	ly = currentY - r / 2;
	ry = currentY + r / 2;

	int plx, ply, prx, pry;
	plx = floor(lx * w[0] + 1e-6);
	prx = floor(rx * w[0] + 1e-6);
	ply = floor(ly * h[0] + 1e-6);
	pry = floor(ry * h[0] + 1e-6);
	clamp(plx, 0, w[0] - 1);
	clamp(prx, 0, w[0] - 1);
	clamp(ply, 0, h[0] - 1);
	clamp(pry, 0, h[0] - 1);

	string filename, filename1;
	string tFile;
	FILE *f, *f1;
	float lastErr;
	const float errThreshold = 2;
	int l, last;

	bool ok[LEVELOFTERRAIN];

	for (int x = plx; x <= prx; x++)
		for (int y = ply; y <= pry; y++) {
				int cx = x * CHUNKREALSIZE + CHUNKREALSIZE / 2;
				int cy = y * CHUNKREALSIZE + CHUNKREALSIZE / 2;
				int vx = currentX * CHUNKNUMBER * CHUNKREALSIZE;
				int vy = currentY * CHUNKNUMBER * CHUNKREALSIZE;

				if (eList[x][y][0] == -1) {
					filename = "data/" + getChunkName(x, y) + "/errList";
					filename1 = "data/" + getChunkName(x, y) + "/averageHeight";
					openfile(filename.c_str(), f);
					openfile(filename1.c_str(), f1);
					for (int i = 0; i < LEVELOFTERRAIN; i++) {
						fread(&eList[x][y][i], sizeof(float), 1, f);
						eList[x][y][i] /= 10;
					}
					fread(&H[x][y], sizeof(float), 1, f1);
					H[x][y] /= 10;
					fclose(f);
					fclose(f1);
				}

				lastErr = 0;
				glm::vec4 p0, p1;
				p0.x = cx - vx;
				p0.z = cy - vy;
				float Dp = sqrt(sqr(p0.x) + sqr(p0.z));
				float s = CHUNKREALSIZE;
				float d = 1 - 1.41421356 * s / 2 / Dp;
				for (int i = 0; i < LEVELOFTERRAIN; i++) {
					if (i > 0 && lastErr <= eList[x][y][i]) {
						lastErr = eList[x][y][i];
						ok[i] = true;
					}
					else {
						ok[i] = false;
					}
				}
				ok[0] = true;

				if (d <= 0) {
					for (int i = 1; i < LEVELOFTERRAIN; i++)
						ok[i] = false;
				}

				last = -1;
				hLevel[y * CHUNKNUMBER + x] = hLevel1[y * CHUNKNUMBER + x] = -1;
				for (int i = LEVELOFTERRAIN - 1; i >= 0; i--) {
					float h;
					if (!ok[i])
						continue;
					l = i;
					h = H[x][y];
					p0.x = cx - vx;
					p0.z = cy - vy;
					p0.y = h - currentH;
					p0 = p0 * d;
					p0.y += currentH;
					p0.w = 1;
					p1 = p0;
					p1.y += eList[x][y][i];
					p0 = matrix * p0;
					p0 /= p0.w;
					p1 = matrix * p1;
					p1 /= p1.w;
					double xPixel = (fabs(p0.x - p1.x) / 2) * WIDTH;
					double yPixel = (fabs(p0.y - p1.y) / 2) * HEIGHT;
					double pixel = xPixel * xPixel + yPixel * yPixel;
					pixel = sqrt(pixel);
					if (pixel <= errThreshold) {
						hLevel[y * CHUNKNUMBER + x] = hLevel1[y * CHUNKNUMBER + x] = i;
						if (last != -1) {
							hLevel1[y * CHUNKNUMBER + x] = last;
							hLevel[y * CHUNKNUMBER + x] += (errThreshold - pixel) / (lastErr - pixel);
						}
						break;
					}
					else {
						last = i;
						lastErr = pixel;
					}
				}
				if (hLevel[y * CHUNKNUMBER + x] == -1) {
					hLevel[y * CHUNKNUMBER + x] = hLevel1[y * CHUNKNUMBER + x] = l = 0;
					last = -1;
				}
			update(l, x >> l, y >> l);
			if (last != -1) {
				l = last;
				update(l, x >> l, y >> l);
			}
		}
}

GLuint VTex::generateTex() {

	glBindTexture(GL_TEXTURE_2D, tex);

	for (int i = 0; i < maxLevel; i++)
		glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA32F_ARB, w[i], h[i], 0,
			GL_RGBA, GL_FLOAT, ptable[i]);

	glBindTexture(GL_TEXTURE_2D, 0);
	return tex;
}

GLuint VTex::getTex() {
	return tex;
}

void VTex::clear() {
	ptex->clearCount();
}


PTex* VTex::getPtex() {
	return ptex;
}

int VTex::getMaxLevel() const {
	return maxLevel;
}

void VTex::map(Page* tp) {
	int level, x, y;
	level = tp->getLevel();
	x = tp->getVx();
	y = tp->getVy();
	float *pt;
	int tx, ty;
	tx = x;
	ty = y;

	for (int i = level; i >= 0; i--) {

		pt = ptable[i] + (ty * w[i] + tx) * 4;
		if (pt[0] >= (float)length[level] / ptex->getPixelSize())
			break;
		int lx, rx, ly, ry;
		lx = tx;
		ly = ty;
		rx = lx + (1 << level - i);
		ry = ly + (1 << level - i);

		for (int k1 = lx; k1 < rx; k1++)

			for (int k2 = ly; k2 < ry; k2++) {
				pt = ptable[i] + (k2 * w[i] + k1) * 4;
				pt[0] = (float)length[level] / ptex->getPixelSize();
				pt[1] = 0;
				pt[2] = (float)tp->getPx() / ptex->getPhysicalWidth() -
					(float)lx / w[i] * pt[0];
				pt[2] += ((float)ptex->getBorderSize())
					/ ptex->getPixelSize();
				pt[3] = (float)tp->getPy() / ptex->getPhysicalWidth() -
					(float)ly / h[i] * pt[0];
				pt[3] += ((float)ptex->getBorderSize())
					/ ptex->getPixelSize();
			}
		ty <<= 1;
		tx <<= 1;
	}
}

void VTex::unmap(Page* tp) {
	int level, x, y;
	level = tp->getLevel();
	if (level == -1)
		return;
	x = tp->getVx();
	y = tp->getVy();
	float *pt;
	int tx, ty;
	tx = x;
	ty = y;
	for (int i = level; i >= 0; i--) {
		pt = ptable[i] + (ty * w[i] + tx) * 4;
		if (pt[0] >= (float)length[level] / ptex->getPixelSize())
			break;
		int lx, rx, ly, ry;
		lx = tx;
		ly = ty;
		rx = lx + (1 << level - i);
		ry = ly + (1 << level - i);
		for (int k1 = lx; k1 < rx; k1++)
			for (int k2 = ly; k2 < ry; k2++) {
				pt = ptable[i] + (k2 * w[i] + k1) * 4;
				pt[0] = 0;
			}
		tx <<= 1;
		ty <<= 1;
	}
	for (int i = level + 1; i < maxLevel; i++) {
		x >>= 1;
		y >>= 1;
		if ((tp = ptex->findPage(i, x, y)) != nullptr) {
			map(tp);
			break;
		}
	}
}

void VTex::loadCoarsest() {
	int level = maxLevel - 1;
	for (int i = 0; i < virtualWidth >> level; i++)
		for (int j = 0; j < virtualWidth >> level; j++) {
			Page *tp = ptex->getReplacePage();
			tp->setVirtualPos(level, i, j);
			tp->loadPage(level, i, j);
			ptex->update(level, i, j, tp);
			ptex->insert(level, i, j, tp);
			map(tp);
		}
}

void VTex::checkThreadState() {
	DWORD exitCode;
	for (int i = 0; i < handleList.size(); i++) {
		GetExitCodeThread(handleList[i].handle, &exitCode);
		if (exitCode != STILL_ACTIVE) {
			//printf("%d\n", exitCode);

			Page* page = handleList[i].page;

			int level, x, y;
			level = page->getLevel();
			x = page->getVx();
			y = page->getVy();
			ptex->update(level, x, y, page);
			ptex->insert(level, x, y, page);
			map(page);
			loadingSet.erase(make_pair(level, y * w[level] + x));
			CloseHandle(handleList[i].handle);
			handleList.erase(handleList.begin() + i);
			i--;
		}
	}
}

unsigned int VTex::updatePage(void* pm) {

	UpdateInfo *uInfo = (UpdateInfo*)pm;

	int level = uInfo->level;
	int x = uInfo->x;
	int y = uInfo->y;
	Page* page = uInfo->page;
	VTex* vtex = uInfo->vtex;
	PTex* ptex = uInfo->ptex;
	SetEvent(event);

	page->setVirtualPos(level, x, y);

	page->loadPage(level, x, y);

	return 1;

}

void VTex::free() {
	CloseHandle(event);
	DeleteCriticalSection(&criticalSection);
}

void VTex::initThread() {
	InitializeCriticalSection(&criticalSection);
}