#pragma once
#include "util.h"
#include "basepage.h"

class SatPage : public BasePage {
private:
	int htop, hoff, ep;

public:
	SatPage();
	~SatPage();
	virtual void loadData(int level, int tx, int ty);
	double getSatVal(int x, int y);
	double getAreaMax(int lx, int ly, int rx, int ry);
};

