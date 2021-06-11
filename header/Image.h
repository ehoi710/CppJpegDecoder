#pragma once

#include <SFML/Graphics.hpp>

#include "utils.h"
#include "Color.h"

class Image {
public:
	virtual Size getSize() const = 0;
	virtual RGBA getPixel(int x, int y) const = 0;
};

void printImage(const Image& image);