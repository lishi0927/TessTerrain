#pragma once
#include "util.h"
#include "basepage.h"

class Page : public BasePage {
private:
	int cellByte;
	string dataname;
	int borderSize;
	int payloadSize;

	float *realdata;

	void loadBorderData(int level, int tx, int ty, int pos);
	void loadmap(int level, int tx, int ty, int pos);
	void loadPage(int level, int x, int y);

	void loadVisPage(int level, int x, int y);
	void loadVismap(int level, int tx, int ty, int pos);
	void loadVisBorderData(int level, int tx, int ty, int pos);

public:
	Page(string dataname);
	~Page();
	virtual void loadData(int level, int x, int y);
};
