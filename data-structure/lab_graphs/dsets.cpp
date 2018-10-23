#include "dsets.h"

void DisjointSets::addelements(int num) {
    // If the number is less than zero, return.
    if (num <= 0) {
        return;
    }

    // Push -1 at the back of the vector.
    for (int i = 0; i < num; i++) {
        disjointsets.push_back(-1);
    }
}


int DisjointSets::find(int elem) {
    // If the element is larger than the size, out of scope, return directly.
    if (elem >= int(disjointsets.size())) {
        return 0;
    }

    return findRootHelperUpdate(elem);
}


int DisjointSets::findRootHelperUpdate(int elem) {
    /**
     * Base case: when the value of the index is less than zero, then this is
     * a root!
     */
    if (disjointsets[elem] < 0) {
        return elem;
    }

    int ret = findRootHelperUpdate(disjointsets[elem]);

    // Update the intermediate children's value to root index.
    disjointsets[elem] = ret;
    return ret;
}


void DisjointSets::setunion(int a, int b) {
    // If a, b are larger than the size, then return directly.
    if (a >= int(disjointsets.size()) || b >= int(disjointsets.size())) {
        return;
    }

    int rootA = findRootHelper(a); // Index of root for a.
    int rootB = findRootHelper(b); // Index of root for b.

    // Special case: If a, b share the same root, then return directly.
    if (rootA == rootB) {
        return;
    }

    int sizeOfRootA = disjointsets[rootA];
    int sizeOfRootB = disjointsets[rootB];

    if (sizeOfRootA > sizeOfRootB) {
        disjointsets[rootA] = rootB;
        disjointsets[rootB] = sizeOfRootA + sizeOfRootB;
    } else {
        // Include the tie-breaker!
        disjointsets[rootB] = rootA;
        disjointsets[rootA] = sizeOfRootA + sizeOfRootB;
    }
}

int DisjointSets::findRootHelper(int elem) const {
    // Base case!
    if (disjointsets[elem] < 0) {
        return elem;
    }

    return findRootHelper(disjointsets[elem]);
}
