#include "basevTex.h"

BaseVTex::BaseVTex() {
}

BaseVTex::~BaseVTex(){
	delete ptex;
}

void BaseVTex::init(int pageSize, int virtualWidth, string dataname, int maxLevel) {
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
		//latest.push_back(new bool[w[i] * h[i]]);
	}

	this->dataname = dataname;
	this->maxLevel = maxLevel;

	ptex = new PTex(pageSize, VIEWCHUNKNUMBER * 0.75, dataname);
	ptex->init(maxLevel, virtualWidth);
	

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

	loadingPtex.clear();
	ptexHandles.clear();
}

HANDLE BaseVTex::event = CreateEvent(nullptr, FALSE, FALSE, nullptr);
CRITICAL_SECTION BaseVTex::criticalSection;

void BaseVTex::updateList(int k, int x, int y, int wk, set<pair<int, int> >&loadingSet, vector<ThreadInfo>&handleList, BaseList* baselist, BaseVTex* vtex) {
	if (loadingSet.find(make_pair(k, y * wk + x)) != loadingSet.end())
		return;
	BasePage *tp;
	tp = baselist->findPage(k, x, y);
	if (tp != nullptr) {
		baselist->moveFront(tp);
	}
	else {
		tp = baselist->getReplacePage();
		if (vtex)
			vtex->unmap(tp);

		UpdateInfo uInfo;
		ThreadInfo tInfo;
		uInfo.level = k;
		uInfo.x = x;
		uInfo.y = y;
		uInfo.page = tInfo.page = tp;
		loadingSet.insert(make_pair(k, y * wk + x));
		tInfo.handle = (HANDLE)_beginthreadex(nullptr, 0, BaseVTex::updatePage, &uInfo, 0, nullptr);
		assert(tInfo.handle != 0);
		handleList.push_back(tInfo);
		WaitForSingleObject(BaseVTex::event, INFINITE);
	}
}

void BaseVTex::update(int k, int x, int y) {
	updateList(k, x, y, w[k], loadingPtex, ptexHandles, ptex, this);
}

GLuint BaseVTex::generateTex() {
	glBindTexture(GL_TEXTURE_2D, tex);

	for (int i = 0; i < maxLevel; i++)
		glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA32F_ARB, w[i], h[i], 0,
			GL_RGBA, GL_FLOAT, ptable[i]);

	glBindTexture(GL_TEXTURE_2D, 0);
	return tex;
}

GLuint BaseVTex::getTex() {
	return tex;
}

void BaseVTex::clear() {
	ptex->clearCount();
}

PTex* BaseVTex::getPtex() {
	return ptex;
}

int BaseVTex::getMaxLevel() const {
	return maxLevel;
}

void BaseVTex::map(BasePage* tp) {
	int level, x, y;
	level = tp->getLevel();
	x = tp->getVx();
	y = tp->getVy();
	float *pt;
	int tx, ty;
	tx = x;
	ty = y;
	//printf("map %d %d %d:\n", level, x, y);
	int tot = 0;
	for (int i = level; i >= 0; i--) {
		int lx, rx, ly, ry;
		int n = CHUNKNUMBER >> i;
		n = (n == 0) ? 1 : n;
		lx = tx;
		ly = ty;
		rx = lx + (1 << level - i);
		clamp(rx, 0, n);
		ry = ly + (1 << level - i);
		clamp(ry, 0, n);
		//printf("level %d range: %d %d %d %d\n\n", i, lx, rx, ly, ry);
		for (int k1 = lx; k1 < rx; k1++)
			for (int k2 = ly; k2 < ry; k2++) {
				pt = ptable[i] + (k2 * w[i] + k1) * 4;
				if (pt[0] >= (float)length[level] / ptex->getPixelSize())
					continue;
				tot++;
				if (i == 0 && k1 == 2 && k2 == 4)
					k1 = k1;
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
		if (!tot)
			break;
		else
			tot = 0;
	}
}

void BaseVTex::unmap(BasePage* tp) {
	int level, x, y;
	level = tp->getLevel();
	if (level == -1)
		return;
	x = tp->getVx();
	y = tp->getVy();
	if (level == 0 && x == 2 && y == 4)
		x = x;
	float *pt;
	int tx, ty;
	tx = x;
	ty = y;
	int tot = 0;
	for (int i = level; i >= 0; i--) {
		int lx, rx, ly, ry;
		int n = CHUNKNUMBER >> i;
		n = (n == 0) ? 1 : n;
		lx = tx;
		ly = ty;
		rx = lx + (1 << level - i);
		clamp(rx, 0, n);
		ry = ly + (1 << level - i);
		clamp(ry, 0, n);
		for (int k1 = lx; k1 < rx; k1++)
			for (int k2 = ly; k2 < ry; k2++) {
				pt = ptable[i] + (k2 * w[i] + k1) * 4;
				if (pt[0] >= (float)length[level] / ptex->getPixelSize())
					continue;
				tot++;
				pt = ptable[i] + (k2 * w[i] + k1) * 4;
				pt[0] = 0;
			}
		tx <<= 1;
		ty <<= 1;
		if (!tot)
			break;
		else
			tot = 0;
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

void BaseVTex::loadCoarsest() {
	int level = maxLevel - 1;
	int n = virtualWidth >> level;
	n = (n == 0) ? 1 : n;
	for (int i = 0; i < n; i++)
		for (int j = 0; j < n; j++) {
			BasePage *tp = ptex->getReplacePage();
			tp->setVirtualPos(level, i, j);
			tp->loadData(level, i, j);
			ptex->update(level, i, j, tp);
			ptex->insert(level, i, j, tp);
			map(tp);
		}
}
void checkState(vector<int>&w, set<pair<int, int> > &loadingSet, vector<ThreadInfo> &handleList, BaseList* baselist, VTex* vtex) {
	DWORD exitCode;
	for (int i = 0; i < handleList.size(); i++) {
		GetExitCodeThread(handleList[i].handle, &exitCode);
		if (exitCode != STILL_ACTIVE) {
			//printf("%d\n", exitCode);
			BasePage* page = handleList[i].page;
			int level, x, y;
			level = page->getLevel();
			x = page->getVx();
			y = page->getVy();
			if (vtex)
				((PTex*)baselist)->update(level, x, y, page);
			baselist->insert(level, x, y, page);
			if (vtex)
				vtex->map(page);
			loadingSet.erase(make_pair(level, y * w[level] + x));
			CloseHandle(handleList[i].handle);
			handleList.erase(handleList.begin() + i);
			i--;
		}
	}
}

void BaseVTex::checkThreadState() {
	checkState(w, loadingPtex, ptexHandles, ptex, this);
}

unsigned int BaseVTex::updatePage(void* pm) {
	//printf("Thread %d begins...\n", GetCurrentThreadId());
	UpdateInfo *uInfo = (UpdateInfo*)pm;
	int level = uInfo->level;
	int x = uInfo->x;
	int y = uInfo->y;
	BasePage* page = uInfo->page;
	SetEvent(event);
	//printf("ThreadID:%d Blocks: %d %d %d %d\n", GetCurrentThreadId(), level, x, y, page->getWidth());
	page->setVirtualPos(level, x, y);
	page->loadData(level, x, y);
	//printf("Thread %d ends...\n", GetCurrentThreadId());
	return 1;
}

void BaseVTex::free() {
	CloseHandle(event);
	DeleteCriticalSection(&criticalSection);
}

void BaseVTex::initThread() {
	InitializeCriticalSection(&criticalSection);
}