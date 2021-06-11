#pragma once

#include <string.h>

#include "Image.h"

class Bitmap : public Image {
public:
	Bitmap(Size bitmap_size, RGBA** pix);
	Bitmap(const Image& image);

	~Bitmap();

public:
	Size getSize() const;
	RGBA getPixel(int x, int y) const;

private:
	Size size;
	RGBA** pixel;
};