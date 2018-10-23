/**
 * @file quadtree.h
 * Quadtree class definition.
 * @date Spring 2008
 */

#ifndef QUADTREE_H
#define QUADTREE_H

#include "png.h"
#include <cmath>
//#include <iostream>

/**
 * A tree structure that is used to compress PNG images.
 */
class Quadtree
{
  public:
      /**
       * Constructs a default Quadtree, which just contains an empty root.
       */
      Quadtree();

      /**
       * Constructs a Quadtree, which will build a Quadtree using given PNG
       * source, and its resolution.
       * @param The source image to base this Quadtree on
       * @param The width and height of the sides of the image to be
       *        represented
       */
      Quadtree(PNG const & source, int resolution);

      /**
       * The copy constructor
       * @param The Quadtree to make a copy of
       */
      Quadtree(Quadtree const & other);

      /**
       * A destructor, free all memory associated with this Quadtree
       */
      ~Quadtree();

      /**
       * Assignment operator; free memory associated with this Quadtree and
       * sets its contents to be equal to parameter's
       * @param A constant reference to the Quadtree value that was copied
       */
      Quadtree const & operator=(Quadtree const & other);

      /**
       * Deletes the current contents of this Quadtree object, then returns
       * it into a Quadtree object representing the upper-left d by d block
       * of source
       * @param The source image to base this Quadtree on
       * @param The width and height of the sides of the image to be
       *        represented
       */
      void buildTree(PNG const & source, int resolution);

      /**
       * Gets the RGBAPixel corresponding to the pixel at coordinates (x, y)
       * in the bitmap image which the Quadtree Represents
       * @param The x coordinate of the pixel to be retrieved
       * @param The y coordinate of the pixel to be retrieved
       */
      RGBAPixel getPixel(int x, int y) const;

      /**
       * Returns the underlying PNG object represented by the Quadtree
       */
      PNG decompress() const;

      /**
       * Rotates the Quadtree object's underlying image clockwise by 90 degrees
       */
      void clockwiseRotate();

      /**
       * Compresses the image this Quadtree Represents
       * @param The integer tolerance between two nodes that determines whether
       *        the subtree can be pruned
       */
      void prune(int tolerance);

      /**
       * It returns a count of the total number of leaves the Quadtree would
       * have if it were pruned as in the prune Functions
       * @param The integer tolerance between two nodes that determines whether
       *        the subtree can be pruned
       */
      int pruneSize(int tolerance) const;

      /**
       * Calculates and returns the minimum tolerance necessary to guarantee
       * that upon pruning the tree, no more than numLeaves leaves remain in
       * the Quadtree
       * @param The number of leaves you want to remain in the tree after
       *        prune is called
       */
      int idealPrune(int numLeaves) const;

  private:
    /**
     * A simple class representing a single node of a Quadtree.
     * You may want to add to this class; in particular, it could
     * probably use a constructor or two...
     */
    class QuadtreeNode
    {
      public:
        QuadtreeNode* nwChild; /**< pointer to northwest child */
        QuadtreeNode* neChild; /**< pointer to northeast child */
        QuadtreeNode* swChild; /**< pointer to southwest child */
        QuadtreeNode* seChild; /**< pointer to southeast child */

        RGBAPixel element; /**< the pixel stored as this node's "data" */
        int xCoord; /**< the x coordinate of the pixel */
        int yCoord; /**< the y coordinate of the pixel */
        int resoOfNode; /** the resolution of the range covered by this
                        QuadtreeNode */

        /**
         * The constructor of the QuadtreeNode, which stores the values
         * of the coordinates of the Pixel.
         * @param The x coordinate of the pixel
         * @param The y coordinate of the pixel
         */
        QuadtreeNode(int x, int y, int theResoOfNode) {
            xCoord = x;
            yCoord = y;
            resoOfNode = theResoOfNode;
        }

        /**
         * Copy constructor of the QuadtreeNode
         * @param The QuadtreeNode that is copying
         */
        QuadtreeNode(QuadtreeNode const * otherNode) {
            xCoord = otherNode->xCoord;
            yCoord = otherNode->yCoord;
            element = otherNode->element;
            resoOfNode = otherNode->resoOfNode;
        }
    };

    /**
     * Copy constructor's helper function
     */
    Quadtree::QuadtreeNode * copy(QuadtreeNode * otherSubRoot);

    /**
     * Clear the whole Quadtree!!!!
     */
    void clear();

    /**
     * Clear's helper function
     */
    void clear(QuadtreeNode * &subRoot);

    /**
     * buildTree's helper function
     */
    void buildTree(QuadtreeNode * subRoot, PNG const & source,
                                                        int resolution);

    /**
     * getPixel's helper function
     */
    RGBAPixel getPixel(QuadtreeNode * subRoot, int x, int y) const;
    
    /**
     * clockwiseRotate's helper function
     */
    void clockwiseRotate(QuadtreeNode * subRoot);

    /**
     * prune's helper function
     */
    void prune(QuadtreeNode * &subRoot, int tolerance);

    /**
     * prune's helper function: to check whether the diff of color is smaller
     * than the tolerance
     */
    bool checkColor(QuadtreeNode * subRoot, QuadtreeNode * subRootChild,
                                                        int tolerance) const;

    /**
     * prune's helper function: to decide whether the subtree need to be
     * pruned
     */
    bool pruneOrNot(QuadtreeNode * subRoot, QuadtreeNode * topRoot,
                                                        int tolerance) const;

    /**
     * pruneSize's helper function
     */
    int pruneSize(QuadtreeNode * subRoot, int tolerance, int counting) const;

    /**
     * idealPrune's helper function
     */
    int idealPrune(int minTolerance, int maxTolerance, int numLeaves) const;

    QuadtreeNode* root; /**< pointer to root of quadtree */


/**** Functions for testing/grading                      ****/
/**** Do not remove this line or copy its contents here! ****/
#include "quadtree_given.h"
};

#endif
