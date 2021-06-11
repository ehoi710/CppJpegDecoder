#include "Color.h"

RGB::RGB() {
	this->R = this->G = this->B = 0x00;
}

/********************************************************************************/

RGBA::RGBA() : RGB() {
	A = 0xFF;
}
RGBA::RGBA(const RGB& rgb) : RGB(rgb) {
	A = 0xFF;
}

/********************************************************************************/

RGB yuvToRgb(YUV yuv) {
	RGB rgb;

	int r = 0, g = 0, b = 0;

	double Y = (double)yuv.Y;
	double Cb = (double)yuv.Cb;
	double Cr = (double)yuv.Cr;

	r = (int)(Y + 1.402 * Cr);
	g = (int)(Y - 0.344 * Cb - 0.714 * Cr);
	b = (int)(Y + 1.772 * Cb);

	rgb.R = betweenCheck(0, r + 128, 255);
	rgb.G = betweenCheck(0, g + 128, 255);
	rgb.B = betweenCheck(0, b + 128, 255);

	return rgb;
}