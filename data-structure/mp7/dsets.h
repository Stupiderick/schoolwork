#ifndef DSETS_H
#define DSETS_H

#include <vector>
#include <iostream>
using namespace std;

class DisjointSets {
public:
    /**
     * Create n unconnected root nodes at the end of the vector.
     * @param the number of the nodes we are gonna add.
     */
    void addelements(int num);

    /**
     * This function should compress the paths.
     * @param the element we are gonna find.
     * @return the index of the root of the up-tree in which the parameter
     *  element resides.
     */
    int find(int elem);

    /**
     * This function should be implemented by union-by-size.
     * @param index of the first element to union.
     * @param index of the second element to union.
     */
    void setunion(int a, int b);

private:
    vector<int> disjointsets; /** The vector as the data structure. */

    /**
     * Recursive helper function for find(int elem), and update the intermediate
     *  children.
     * @param the element that we are gonna find.
     * @return the root of the element.
     */
    int findRootHelperUpdate(int elem);

    /**
     * Recursive helper function to find the root of the element.
     * @param the element that we are gonna find.
     * @return the root of the element.
     */
    int findRootHelper(int elem) const;
};

#endif
