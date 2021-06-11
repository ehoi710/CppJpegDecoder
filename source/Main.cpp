#include "Image.h"
#include "Bitmap.h"
#include "JPEG.h"

int main(int argc, char* argv[]) {
	if (argc < 2) {
		printf("<EJPEG>: this program need two parameters.\n");
		exit(-1);
	}
	const char* file_path = argv[1];
	EJPEG::JPEG jp(file_path);
	jp.decode();

	printImage(jp);

	return 0;
}