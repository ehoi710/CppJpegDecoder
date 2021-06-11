#pragma once

#include <stdio.h>

#include <vector>
#include <string>

typedef unsigned char uint8;
typedef unsigned short uint16;
typedef unsigned int uint32;

struct Size {
	int x, y;
};

struct Bound {
	int start, end;
};

size_t		getFileSize(FILE* fp);
uint8*		readFile(FILE* fp);
uint16		getWord(std::vector<uint8>::iterator iter);
int			betweenCheck(int min, int val, int max);
std::string	toBitString(int value);