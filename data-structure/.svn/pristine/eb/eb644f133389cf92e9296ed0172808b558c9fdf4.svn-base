/**
 * @file quadtree.cpp
 * Quadtree class implementation.
 * @date Spring 2008
 */
#include<iostream>
#include "quadtree.h"
using namespace std;
Quadtree::Quadtree() {
    root = NULL;
}


Quadtree::Quadtree(PNG const & source, int resolution) {
    root = new QuadtreeNode(0, 0, resolution);
    buildTree(root, source, resolution);
}


Quadtree::Quadtree(Quadtree const & other) {
    root = copy(other.root);
}


Quadtree::~Quadtree() {
    clear();
}


Quadtree const & Quadtree::operator=(Quadtree const & other) {
    if (this != &other) {
        clear();
        root = copy(other.root);
    }

    return *this;
}

Quadtree::QuadtreeNode * Quadtree::copy(Quadtree::QuadtreeNode * otherSubRoot) {
    if (otherSubRoot == NULL) {
        return NULL;
    }

    QuadtreeNode * curr = new QuadtreeNode(otherSubRoot);

    curr->nwChild = copy(otherSubRoot->nwChild);
    curr->neChild = copy(otherSubRoot->neChild);
    curr->swChild = copy(otherSubRoot->swChild);
    curr->seChild = copy(otherSubRoot->seChild);

    return curr;
}

void Quadtree::clear() {
    clear(root);
}


void Quadtree::clear(QuadtreeNode * &subRoot) {
    if (subRoot == NULL) {
        return;
    }

    clear(subRoot->nwChild);
    clear(subRoot->neChild);
    clear(subRoot->swChild);
    clear(subRoot->seChild);

    delete subRoot;
    subRoot = NULL;
}


void Quadtree::buildTree(PNG const & source, int resolution) {
    clear(root);
    root = new QuadtreeNode(0, 0, resolution);
    buildTree(root, source, resolution);
}


void Quadtree::buildTree(QuadtreeNode * subRoot, PNG const & source,
                                                    int resolution) {
    if (resolution == 1) {
        subRoot->element = *(source(subRoot->xCoord, subRoot->yCoord));
        subRoot->nwChild = NULL;
        subRoot->neChild = NULL;
        subRoot->swChild = NULL;
        subRoot->seChild = NULL;
        return;
    }

    subRoot->nwChild = new QuadtreeNode(subRoot->xCoord, subRoot->yCoord,
                                                            resolution / 2);
    subRoot->neChild = new QuadtreeNode((subRoot->xCoord) + resolution / 2,
                                            subRoot->yCoord, resolution / 2);
    subRoot->swChild = new QuadtreeNode(subRoot->xCoord,
                        (subRoot->yCoord) + resolution / 2, resolution / 2);
    subRoot->seChild = new QuadtreeNode((subRoot->xCoord) + resolution / 2,
                        (subRoot->yCoord) + resolution / 2, resolution / 2);

    buildTree(subRoot->nwChild, source, resolution / 2);
    buildTree(subRoot->neChild, source, resolution / 2);
    buildTree(subRoot->swChild, source, resolution / 2);
    buildTree(subRoot->seChild, source, resolution / 2);

    subRoot->element.red = (subRoot->nwChild->element.red
                          + subRoot->neChild->element.red
                          + subRoot->swChild->element.red
                          + subRoot->seChild->element.red) / 4;

    subRoot->element.green = (subRoot->nwChild->element.green
                            + subRoot->neChild->element.green
                            + subRoot->swChild->element.green
                            + subRoot->seChild->element.green) / 4;

    subRoot->element.blue = (subRoot->nwChild->element.blue
                           + subRoot->neChild->element.blue
                           + subRoot->swChild->element.blue
                           + subRoot->seChild->element.blue) / 4;
}


RGBAPixel Quadtree::getPixel(int x, int y) const {
    if (x >= (root->resoOfNode)) {
        return RGBAPixel();
    }

    if (y >= (root->resoOfNode)) {
        return RGBAPixel();
    }

    if (x < 0 || y < 0) {
        return RGBAPixel();
    }

    return getPixel(root, x, y);
}


RGBAPixel Quadtree::getPixel(QuadtreeNode * subRoot, int x, int y) const {
    if (subRoot->nwChild == NULL) {
        return subRoot->element;
    }

    if (x < ((subRoot->xCoord) + (subRoot->resoOfNode) / 2)) {
        if (y < ((subRoot->yCoord) + (subRoot->resoOfNode) / 2)) {
            return getPixel(subRoot->nwChild, x, y);
        } else {
            return getPixel(subRoot->swChild, x, y);
        }
    } else {
        if (y < ((subRoot->yCoord) + (subRoot->resoOfNode) / 2)) {
            return getPixel(subRoot->neChild, x, y);
        } else {
            return getPixel(subRoot->seChild, x, y);
        }
    }
}


PNG Quadtree::decompress() const {
    if (root == NULL) {
        return PNG();
    }

    PNG ret = PNG(root->resoOfNode, root->resoOfNode);

    for (int i = 0; i < root->resoOfNode; i++) {
        for (int j = 0; j < root->resoOfNode; j++) {
            *(ret(i, j)) = getPixel(i, j);
        }
    }

    return ret;
}


void Quadtree::clockwiseRotate() {
    if (root == NULL) {
        return;
    }

    clockwiseRotate(root);
}


void Quadtree::clockwiseRotate(QuadtreeNode * subRoot) {
    if (subRoot->nwChild == NULL) {
        return;
    }

    QuadtreeNode * temp;
    temp = subRoot->nwChild;
    subRoot->nwChild = subRoot->swChild;
    subRoot->swChild = subRoot->seChild;
    subRoot->seChild = subRoot->neChild;
    subRoot->neChild = temp;

    subRoot->nwChild->xCoord = subRoot->xCoord;
    subRoot->neChild->xCoord = subRoot->xCoord + (subRoot->resoOfNode) / 2;
    subRoot->seChild->xCoord = subRoot->xCoord + (subRoot->resoOfNode) / 2;
    subRoot->swChild->xCoord = subRoot->xCoord;

    subRoot->nwChild->yCoord = subRoot->yCoord;
    subRoot->neChild->yCoord = subRoot->yCoord;
    subRoot->seChild->yCoord = subRoot->yCoord + (subRoot->resoOfNode) / 2;
    subRoot->swChild->yCoord = subRoot->yCoord + (subRoot->resoOfNode) / 2;

    temp = NULL;

    clockwiseRotate(subRoot->nwChild);
    clockwiseRotate(subRoot->neChild);
    clockwiseRotate(subRoot->swChild);
    clockwiseRotate(subRoot->seChild);
}


void Quadtree::prune(int tolerance) {
    if (root == NULL) {
        return;
    }

    prune(root, tolerance);
}


void Quadtree::prune(QuadtreeNode * &subRoot, int tolerance) {
    if (subRoot->nwChild == NULL) {
        return;
    }

    if (pruneOrNot(subRoot, subRoot, tolerance)) {
        clear(subRoot->nwChild);
        clear(subRoot->neChild);
        clear(subRoot->swChild);
        clear(subRoot->seChild);

    } else {
        prune(subRoot->nwChild, tolerance);
        prune(subRoot->neChild, tolerance);
        prune(subRoot->swChild, tolerance);
        prune(subRoot->seChild, tolerance);
    }
}


bool Quadtree::pruneOrNot(QuadtreeNode * subRoot, QuadtreeNode * topRoot,
                                                        int tolerance) const {
    if (subRoot == NULL) {
        return true;
    }

    bool checkOkay = pruneOrNot(subRoot->nwChild, topRoot, tolerance)
        && pruneOrNot(subRoot->neChild, topRoot, tolerance)
        && pruneOrNot(subRoot->swChild, topRoot, tolerance)
        && pruneOrNot(subRoot->seChild, topRoot, tolerance);

    if (subRoot->nwChild != NULL) {
        return checkOkay;
    }

    return checkColor(subRoot, topRoot, tolerance);
}


bool Quadtree::checkColor(QuadtreeNode * subRoot, QuadtreeNode * subRootChild,
                                                        int tolerance) const {
    return pow(subRoot->element.red - subRootChild->element.red, 2)
            + pow(subRoot->element.green - subRootChild->element.green, 2)
            + pow(subRoot->element.blue - subRootChild->element.blue, 2)
            <= tolerance;
}


int Quadtree::pruneSize(int tolerance) const {
    if (root == NULL) {
        return 0;
    }

    return pruneSize(root, tolerance, 0);
}


int Quadtree::pruneSize(QuadtreeNode * subRoot, int tolerance, int counting)
                                                                        const {
    if (subRoot->nwChild == NULL) {
        return counting + 1;
    }

    if (pruneOrNot(subRoot, subRoot, tolerance)) {
        counting++;

    } else {
        counting = pruneSize(subRoot->nwChild, tolerance, counting);
        counting = pruneSize(subRoot->neChild, tolerance, counting);
        counting = pruneSize(subRoot->swChild, tolerance, counting);
        counting = pruneSize(subRoot->seChild, tolerance, counting);
    }

    return counting;
}


int Quadtree::idealPrune(int numLeaves) const {
    if (root == NULL) {
        return 0;
    }

    int maxTolerance = 225 * 225 * 3;

    return idealPrune(0, maxTolerance, numLeaves);
}


int Quadtree::idealPrune(int minTolerance, int maxTolerance, int numLeaves)
                                                                        const {
    int midTolerance = (minTolerance + maxTolerance) / 2;
    int midNumLeaves = pruneSize(midTolerance);

    if (maxTolerance - minTolerance == 1) {
        if (midNumLeaves == numLeaves) {
            return midTolerance;
        } else {
            return maxTolerance;
        }
    }

    if (midNumLeaves > numLeaves) {
        return idealPrune(midTolerance, maxTolerance, numLeaves);
    }

    return idealPrune(minTolerance, midTolerance, numLeaves);
}
