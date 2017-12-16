#include "util.h"
#include "page.h"
#include "pTex.h"

class ThreadInfo
{
public:
	BasePage* page;
	HANDLE handle;
	BasePage* satPage;
};

class BaseVTex {
public:
	vector<float*> ptable;
	vector<int> w, h;
	vector<int> length;
	GLuint tex;
	PTex* ptex;
	int maxLevel;
	string dataname;
	int pageSize, virtualWidth;
	set<pair<int, int> > loadingPtex;
	vector<ThreadInfo> ptexHandles;

	static HANDLE event;
	static CRITICAL_SECTION criticalSection;
	BaseVTex();
	~BaseVTex();
	void init(int pageSize, int virtualWidth, string dataname, int maxLevel);
	void updateList(int k, int x, int y, int wk, set<pair<int, int> >&loadingSet, vector<ThreadInfo>&handleList, BaseList* baselist, BaseVTex* vtex);
	void update(int k, int x, int y);
	GLuint generateTex();
	GLuint getTex();
	void clear();
	PTex * getPtex();
	int getMaxLevel() const;
	void map(BasePage * tp);
	void unmap(BasePage * tp);
	void loadCoarsest();
	void checkThreadState();
	static unsigned int __stdcall updatePage(void * pm);
	static void free();
	static void initThread();
};

class UpdateInfo {
public:
	int level, x, y;
	BasePage* page;
};