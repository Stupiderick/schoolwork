#ifndef IMAGE_H
#define IMAGE_H

#include <iostream>
#include "png.h"
#include "rgbapixel.h"

using std::uint8_t;

class Image : public PNG {
public:
	using PNG::PNG;
	void flipleft();
	void adjustbrightness(int r, int g, int b);
	void invertcolors();
};

#endif
