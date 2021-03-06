#ifndef DDS_LOADER_H
#define DDS_LOADER_H

#include "util.h"
#include "dds_reader.h"

using namespace std;

class DDSLoader {
private:
	int width, height;
	unsigned char* data;
	int imageSize;
	void loadBorderData(int level, int index, int x, int y, string dataname);
	int getWidth(FILE *fp);
	int getHeight(FILE *fp);

public:
	DDSLoader();
	~DDSLoader();

	void getTotalBlock(const char *filename, int mipmap, int &width, int &height,
		int* &pixels);

	void loadCompressedData(int level, int index, int x, int y,
		string dataname);

	int getWidth() const;
	int getHeight() const;
	unsigned char* getData() const;
	int getImageSize() const;
};

#endif