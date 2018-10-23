/**
 * @file filler.cpp
 * Implementation of functions in the filler namespace. Heavily based on
 * old MP4 by CS225 Staff, Fall 2010.
 *
 * @author Chase Geigle
 * @date Fall 2012
 */
#include "filler.h"

animation filler::dfs::fillSolid(PNG& img, int x, int y, RGBAPixel fillColor,
                                 int tolerance, int frameFreq)
{
     solidColorPicker fillingSolid(fillColor);
     return filler::dfs::fill(img, x, y, fillingSolid, tolerance, frameFreq);
}


animation filler::dfs::fillGrid(PNG& img, int x, int y, RGBAPixel gridColor,
                                int gridSpacing, int tolerance, int frameFreq)
{
     gridColorPicker fillingGrid(gridColor, gridSpacing);
     return filler::dfs::fill(img, x, y, fillingGrid, tolerance, frameFreq);
}


animation filler::dfs::fillGradient(PNG& img, int x, int y,
                                    RGBAPixel fadeColor1, RGBAPixel fadeColor2,
                                    int radius, int tolerance, int frameFreq)
{
     gradientColorPicker fillingGradient(fadeColor1, fadeColor2, radius, x, y);
     return filler::dfs::fill(img, x, y, fillingGradient, tolerance, frameFreq);
}


animation filler::dfs::fill(PNG& img, int x, int y, colorPicker& fillColor,
                            int tolerance, int frameFreq)
{
     return filler::fill<Stack>(img, x, y, fillColor, tolerance, frameFreq);
}


animation filler::bfs::fillSolid(PNG& img, int x, int y, RGBAPixel fillColor,
                                 int tolerance, int frameFreq)
{
     solidColorPicker fillingSolid(fillColor);
     return filler::bfs::fill(img, x, y, fillingSolid, tolerance, frameFreq);
}


animation filler::bfs::fillGrid(PNG& img, int x, int y, RGBAPixel gridColor,
                                int gridSpacing, int tolerance, int frameFreq)
{
     gridColorPicker fillingGrid(gridColor, gridSpacing);
     return filler::bfs::fill(img, x, y, fillingGrid, tolerance, frameFreq);
}


animation filler::bfs::fillGradient(PNG& img, int x, int y,
                                    RGBAPixel fadeColor1, RGBAPixel fadeColor2,
                                    int radius, int tolerance, int frameFreq)
{
     gradientColorPicker fillingGradient(fadeColor1, fadeColor2, radius, x, y);
     return filler::bfs::fill(img, x, y, fillingGradient, tolerance, frameFreq);
}


animation filler::bfs::fill(PNG& img, int x, int y, colorPicker& fillColor,
                            int tolerance, int frameFreq)
{
     return filler::fill<Queue>(img, x, y, fillColor, tolerance, frameFreq);
}


template <template <class T> class OrderingStructure>
animation filler::fill(PNG& img, int x, int y, colorPicker& fillColor,
                       int tolerance, int frameFreq)
{
    /**
     * This is the basic description of a flood-fill algorithm: Every fill
     * algorithm requires an ordering structure, which is passed to this
     * function via its template parameter. For a breadth-first-search
     * fill, that structure is a Queue, for a depth-first-search, that
     * structure is a Stack. To begin the algorithm, you simply place the
     * given point in the ordering structure. Then, until the structure is
     * empty, you do the following:
     *
     * 1.     Remove a point from the ordering structure.
     *
     *        If it has not been processed before and if its color is
     *        within the tolerance distance (up to and **inpow(cluding**
     *        tolerance away in square-RGB-space-distance) to the original
     *        point's pixel color [that is, \f$(currentRed - OriginalRed)^2 +
              (currentGreen - OriginalGreen)^2 + (currentBlue -
              OriginalBlue)^2 \leq tolerance\f$], then:
     *        1.    indicate somehow that it has been processed (do not mark it
     *              "processed" anywhere else in your code)
     *        2.    change its color in the image using the appropriate
     *              colorPicker
     *        3.    add all of its neighbors to the ordering structure, and
     *        4.    if it is the appropriate frame, send the current PNG to the
     *              animation (as described below).
     * 2.     When implementing your breadth-first-search and
     *        depth-first-search fills, you will need to explore neighboring
     *        pixels in some order.
     *
     *        While the order in which you examine neighbors does not matter
     *        for a proper fill, you must use the same order as we do for
     *        your animations to come out like ours! The order you should put
     *        neighboring pixels **ONTO** the queue or stack is as follows:
     *        **RIGHT(+x), DOWN(+y), LEFT(-x), UP(-y). IMPORTANT NOTE: *UP*
     *        here means towards the top of the image, so since an image has
     *        smaller y coordinates at the top, this is in the *negative y*
     *        direction. Similarly, *DOWN* means in the *positive y*
     *        direction.** To reiterate, when you are exploring (filling out)
     *        from a given pixel, you must first try to fill the pixel to
     *        it's RIGHT, then the one DOWN from it, then to the LEFT and
     *        finally UP. If you do them in a different order, your fill may
     *        still work correctly, but your animations will be different
     *        from the grading scripts!
     * 3.     For every k pixels filled, **starting at the kth pixel**, you
     *        must add a frame to the animation, where k = frameFreq.
     *
     *        For example, if frameFreq is 4, then after the 4th pixel has
     *        been filled you should add a frame to the animation, then again
     *        after the 8th pixel, etc.  You must only add frames for the
     *        number of pixels that have been filled, not the number that
     *        have been checked. So if frameFreq is set to 1, a pixel should
     *        be filled every frame.
     */

     OrderingStructure<int> coorX;
     OrderingStructure<int> coorY;
     int frameCounting = 0;
     animation myAnimation;

     int widthOfImg = int(img.width());
     int heightOfImg = int(img.height());

    bool * itHasBeenSearched = new bool[widthOfImg * heightOfImg];
    for (int i = 0; i < widthOfImg * heightOfImg; i++) {
        itHasBeenSearched[i] = false;
    }

     coorX.add(x);
     coorY.add(y);

     int origRed = img(x, y)->red;
     int origGreen = img(x, y)->green;
     int origBlue = img(x, y)->blue;

     while (!coorX.isEmpty() && !coorY.isEmpty()) {
         int valCoorX = coorX.remove();
         int valCoorY = coorY.remove();

         int currRed = img(valCoorX, valCoorY)->red;
         int currGreen = img(valCoorX, valCoorY)->green;
         int currBlue = img(valCoorX, valCoorY)->blue;

         int diffRed = origRed - currRed;
         int diffGreen = origGreen - currGreen;
         int diffBlue = origBlue - currBlue;

         bool tole = (pow(diffRed, 2) + pow(diffGreen, 2) + pow(diffBlue, 2)) <= tolerance;

         if (!itHasBeenSearched[widthOfImg * valCoorY + valCoorX] && tole) {
             itHasBeenSearched[widthOfImg * valCoorY + valCoorX] = true;

             RGBAPixel temp = fillColor(valCoorX, valCoorY);
             img(valCoorX, valCoorY)->red = temp.red;
             img(valCoorX, valCoorY)->green = temp.green;
             img(valCoorX, valCoorY)->blue = temp.blue;

             //Going right
             if (valCoorX + 1 < widthOfImg) {
                 coorX.add(valCoorX + 1);
                 coorY.add(valCoorY);
             }

             //Going down
             if (valCoorY + 1 < heightOfImg) {
                 coorX.add(valCoorX);
                 coorY.add(valCoorY + 1);
             }

             //Going left
             if (valCoorX - 1 >= 0) {
                 coorX.add(valCoorX - 1);
                 coorY.add(valCoorY);
             }

             //Going up
             if (valCoorY - 1 >= 0) {
                 coorX.add(valCoorX);
                 coorY.add(valCoorY - 1);
             }

             frameCounting++;
             if (frameCounting % frameFreq == 0) {
                 myAnimation.addFrame(img);
             }
         }
     }

     delete[] itHasBeenSearched;
     return myAnimation;
}
