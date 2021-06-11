#include "Bitmap.h"

Bitmap::Bitmap(Size bitmap_size, RGBA** pix) {
	size = bitmap_size;
	pixel = new RGBA*[size.y];
	for (int j = 0; j < size.y; j++) {
		pixel[j] = new RGBA[size.x];
		memcpy(pixel[j], pix[j], size.x * sizeof(RGBA));
	}
}
Bitmap::Bitmap(const Image& image) {
	size = image.getSize();

	pixel = new RGBA*[size.y];
	for (int y = 0; y < size.y; y++) {
		pixel[y] = new RGBA[size.x];
		for (int x = 0; x < size.x; x++) {
			pixel[y][x] = image.getPixel(x, y);
		}
	}
}

Bitmap::~Bitmap() {
	for (int i = 0; i < size.y; i++) {
		delete[] pixel[i];
	}
	delete[] pixel;
}

Size Bitmap::getSize() const {
	return size;
}
RGBA Bitmap::getPixel(int x, int y) const {
	return pixel[y][x];
}

/********************************************************************************/
