#pragma once

#include <SFML/Graphics.hpp>

#include "Utils.h"
#include "Color.h"

// Image에 대한 인터페이스.
class Image {
public:
	virtual Size getSize() const = 0;
	virtual RGBA getPixel(int x, int y) const = 0;
};

void printImage(const Image& image);