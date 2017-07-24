#ifndef BASELIST_H
#define BASELIST_H

#include "util.h"
#include "basepage.h"


class BaseList {
protected:
	BasePage *head, *tail;
    vector<vector<BasePage *> >pagePos;
	int maxSize;
	int count;
	int virtualWidth;
	int level;

public:
	BaseList();
	~BaseList();
	virtual void init(int level, int virtualWidth) {};
	void moveFront(BasePage * p);
	void insert(int level, int x, int y, BasePage * p);
	BasePage * getReplacePage();
	BasePage * findPage(int level, int x, int y);
	void clearCount();
};

#endif