#include "utils.h"

size_t		getFileSize(FILE* fp) {
	fseek(fp, 0, SEEK_END);
	int len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	return len;
}
uint8*		readFile(FILE* fp) {
	int len = getFileSize(fp);
	uint8* data = new uint8[len];
	fread(data, sizeof(uint8), len, fp);

	return data;
}
uint16		getWord(std::vector<uint8>::iterator iter) {
	return (iter[0] * 0x100) + iter[1];
}
int			betweenCheck(int min, int val, int max) {
	if (val < min) return min;
	if (val > max) return max;
	return val;
}
std::string toBitString(int value) {
	std::string res("");
	int mask = 0;
	while ((1 << mask) < value) mask++;

	while (mask >= 0) {
		res.push_back((value >> mask) + '0');
		value &= ~(1 << mask);
		mask--;
	}

	return res;
}