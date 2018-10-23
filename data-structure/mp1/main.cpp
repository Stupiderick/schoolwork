#include <iostream>
#include <algorithm>
using namespace std;

#include "rgbapixel.h"
#include "png.h"

int main () {
	PNG image("in.png");      //Input image from in.png
	PNG nImage(image.width(), image.height());   //Create a new image file with the same height and width as in.png

	for (size_t row = 0; row < image.width(); row ++) {
		for (size_t column = 0; column < image.height(); column ++) {
			nImage (row, column)->red = image (image.width() - row - 1, image.height() - column - 1)->red;
			nImage (row, column)->green = image (image.width() - row - 1, image.height() - column - 1)->green;
			nImage (row, column)->blue = image (image.width() - row - 1, image.height() - column - 1)->blue;
			nImage (row, column)->alpha = image (image.width() - row - 1, image.height() - column - 1)->alpha;   //Rotate the image pixels with 180 degree
		}
	}

	nImage.writeToFile("out.png");   //Output the manipulated image to out.png

	return 0;
}
