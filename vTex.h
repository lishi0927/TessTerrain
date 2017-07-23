#ifndef VTEX_H
#define VTEX_H

#include "util.h"
#include "page.h"
#include "pTex.h"
#include "sat.h"

class ThreadInfo {
public:
	Page* page;
	HANDLE handle;
};

class VTex {
private:
	vector<float*> ptable;

	vector<int> w, h;

	vector<int> length;
	GLuint tex;
	PTex* ptex;

	int pageSize, virtualWidth;

	int maxLevel;
	bool isHeightmap;
	set<pair<int, int> > loadingSet;
	vector<ThreadInfo> handleList;
	static HANDLE event;
	static CRITICAL_SECTION criticalSection;

public:
	VTex();
	~VTex();
	void init(int pageSize, int virtualWidth, bool isHeightmap, int maxLevel);
	void update(int k, int x, int y);
	void update(float currentX, float currentY, float currentH, float hLevel[], unsigned char hLevel1[],
		glm::mat4 matrix);
	GLuint generateTex();
	GLuint getTex();
	void clear();
	PTex * getPtex();
	int getMaxLevel() const;

	void map(Page * tp);
	void unmap(Page * tp);
	void loadCoarsest();
	void checkThreadState();
	static unsigned int __stdcall updatePage(void * pm);
	static void free();
	static void initThread();
};

class UpdateInfo {
public:
	int level, x, y;
	Page* page;
	VTex* vtex;
	PTex* ptex;
};

#endif