#pragma once

#include "Utils.h"

struct YUV {
	int Y, Cb, Cr;
};

struct RGB {
	uint8 R, G, B;

	RGB();
};
struct RGBA : RGB {
	uint8 A;

	RGBA();
	RGBA(const RGB& rgb);
};

RGB yuvToRgb(YUV yuv);