#include "util.h"

string getChunkName(int offsetx, int offsety) {
	string tempx = to_string(offsetx), tempy = to_string(offsety);
	string newdirectory = tempx + string("_") + tempy;
	return newdirectory;
}

void clamp(int &x, int l, int r) {
	assert(l <= r);
	x = (x < l) ? l : x;
	x = (x > r) ? r : x;
}

float clampf(float x, float a, float b)
{
	if (x > b)return b;
	else if (x < a) return a;
	else return x;
}

void openfile(const char* filename, FILE* &fp) {
	if ((fp = fopen(filename, "rb")) == nullptr) {
		printf("Cannot open file.\n");
		//exit(1);
	}
}

void seekfile(FILE* fp, int off, int pos) {
	assert(pos >= 0 && pos <= 2);
	if (fseek(fp, off, 0)) {
		printf("file seek failed.\n");
		exit(1);
	}
}

uint64_t getRepresentation(const double number) {
	uint64_t representation = 0;
	memcpy(&representation, &number, sizeof(representation));
	return representation;
}