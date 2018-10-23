#include "image.h"
#include <cmath>
using namespace std;

void Image::flipleft() {
	size_t height = this->height();
	size_t width = this->width();
	size_t row, column;

	for (row = 0; row < height; row++) {
		for (column = 0; column < floor(width / 2); column++) {   //floor() is used to make rounding-down.
			int tempRed, tempGreen, tempBlue;

			tempRed = (*this)(column, row)->red;
			(*this)(column, row)->red = (*this)(width - column - 1, row)->red;
			(*this)(width - column - 1, row)->red = tempRed;

			tempGreen = (*this)(column, row)->green;
			(*this)(column, row)->green = (*this)(width - column- 1, row)->green;
			(*this)(width - column- 1, row)->green = tempGreen;

			tempBlue = (*this)(column, row)->blue;
			(*this)(column, row)->blue = (*this)(width - column- 1, row)->blue;
			(*this)(width - column- 1, row)->blue = tempBlue;
		}
	}
}

void Image::adjustbrightness(int r, int g, int b) {
	size_t height = this->height();
	size_t width = this->width();
	size_t row, column;

	for (row = 0; row < height; row++) {
		for (column = 0; column < width; column++) {
			int temp;    //Because size_t is unsighed, so I need to create an int type which is sighed, in case the RGB values overflow due to negative values.

			temp = (*this)(column, row)->red + r;
			if (temp < 0)
				temp = 0;
			if (temp > 255)
				temp = 255;
			(*this)(column, row)->red = temp;

			temp = (*this)(column, row)->green + g;
			if (temp < 0)
				temp = 0;
			if (temp > 255)
				temp = 255;
			(*this)(column, row)->green = temp;

			temp = (*this)(column, row)->blue + b;
			if (temp < 0)
				temp = 0;
			if (temp > 255)
				temp = 255;
			(*this)(column, row)->blue = temp;
		}
	}
}

void Image::invertcolors () {
	size_t height = this->height();
	size_t width = this->width();
	size_t row, column;

	for (row = 0; row < height; row++) {
		for (column = 0; column < width; column++) {
			(*this)(column, row)->red = 255 - (*this)(column, row)->red;
			(*this)(column, row)->green = 255 - (*this)(column, row)->green;
			(*this)(column, row)->blue = 255 - (*this)(column, row)->blue;
		}
	}
}
