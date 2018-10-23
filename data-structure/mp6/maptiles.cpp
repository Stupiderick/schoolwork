/**
 * @file maptiles.cpp
 * Code for the maptiles function.
 */

#include <iostream>
#include <map>
#include "maptiles.h"

using namespace std;

MosaicCanvas* mapTiles(SourceImage const& theSource,
                       vector<TileImage> const& theTiles)
{
    int myRow = theSource.getRows(); //get the row of the source image
    int myCol = theSource.getColumns(); //Get the column of the source image

    MosaicCanvas * myMosaicCanvas = new MosaicCanvas(myRow, myCol);

    /** Get the size of the vector storing the tile image */
    unsigned sizeOfTiles = theTiles.size();
    std::map<Point<3>, TileImage> myMap;
    std::vector<Point<3>> vecOfPoints;

    for (unsigned i = 0; i < sizeOfTiles; i++) {
        /** Get the average color of every single tile image in the vector */
        RGBAPixel aveColor = theTiles[i].getAverageColor();
        double arrOfRGB[3] = {double(aveColor.red),
                              double(aveColor.green),
                              double(aveColor.blue)};
        Point<3> pointOfTiles(arrOfRGB); //Store the average color into Point.
        vecOfPoints.push_back(pointOfTiles);

        myMap[pointOfTiles] = theTiles[i];
    }

    /**
     * Construct a KDTree storing the Points which store the average color of
     * every single tile image in the vector.
     */
    KDTree<3> myKDTree(vecOfPoints);

    for (int i = 0; i < myRow; i++) {
        for (int j = 0; j < myCol; j++) {
            /** Get the region average color of the source image. */
            RGBAPixel pixelInSource = theSource.getRegionColor(i, j);
            double arrOfTarget[3] = {double(pixelInSource.red),
                                     double(pixelInSource.green),
                                     double(pixelInSource.blue)};

            /** Put the region average color into Points. */
            Point<3> target(arrOfTarget);
            /** Get the nearest neighbor using the function. */
            Point<3> singleTile = myKDTree.findNearestNeighbor(target);
            /** Put the qualified image into the mosaic canvas. */
            myMosaicCanvas->setTile(i ,j, myMap[singleTile]);
        }
    }

    return myMosaicCanvas;
}
