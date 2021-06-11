#pragma once

#include <SFML/Graphics.hpp>

#include "Utils.h"
#include "Color.h"

// Image�� ���� �������̽�.
class Image {
public:
	virtual Size getSize() const = 0;
	virtual RGBA getPixel(int x, int y) const = 0;
};

void printImage(const Image& image);