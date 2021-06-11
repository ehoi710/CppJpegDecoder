#include "Image.h"

void printImage(const Image& image) {
	Size size = image.getSize();

	sf::RenderWindow window(sf::VideoMode(size.x, size.y), "SFML works!");

	sf::Texture texture;
	texture.create(size.x, size.y);

	sf::Sprite sprite(texture);

	sf::Uint8* pixels = new sf::Uint8[size.x * size.y * 4];

	for (int i = 0; i < size.x * size.y * 4; i += 4) {
		int raw_i = i / 4;
		RGBA pix = image.getPixel(raw_i % size.x, raw_i / size.x);

		pixels[i] = pix.R;
		pixels[i + 1] = pix.G;
		pixels[i + 2] = pix.B;
		pixels[i + 3] = pix.A;
	}

	texture.update(pixels);

	while (window.isOpen()) {
		sf::Event event;
		while (window.pollEvent(event)) {
			if (event.type == sf::Event::Closed) {
				window.close();
			}
		}

		window.clear();
		window.draw(sprite);
		window.display();
	}

	return;
}