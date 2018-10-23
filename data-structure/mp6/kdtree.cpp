/**
 * @file kdtree.cpp
 * Implementation of KDTree class.
 */

template <int Dim>
bool KDTree<Dim>::smallerDimVal(const Point<Dim>& first,
                                const Point<Dim>& second, int curDim) const
{
    if (curDim > Dim) {
        return false;
    }

    if (first[curDim] < second[curDim]) {
        return true;
    } else if (first[curDim] > second[curDim]) {
        return false;
    } else {
        return first < second;
    }
}

template <int Dim>
bool KDTree<Dim>::shouldReplace(const Point<Dim>& target,
                                const Point<Dim>& currentBest,
                                const Point<Dim>& potential) const
{
    int distanceCurr = findDistance(currentBest, target);
    int distancePotential = findDistance(potential, target);

    if (distanceCurr > distancePotential) {
        return true;
    } else if (distanceCurr < distancePotential){
        return false;
    } else {
        return (potential < currentBest);
    }
}

template <int Dim>
int KDTree<Dim>::findDistance(const Point<Dim>& target,
                                            const Point<Dim>& sample) const {
    int distance = 0;
    for (int i = 0; i < Dim; i++) {
        distance += pow(sample[i] - target[i], 2);
    }
    return distance;
}

template <int Dim>
KDTree<Dim>::KDTree(const vector<Point<Dim>>& newPoints)
{
    points = newPoints;
    int vecSize = points.size();
    buildTree(0, vecSize - 1, 0);
}

template <int Dim>
void KDTree<Dim>::buildTree(int left, int right, int dimension) {
    /**
     * If the right index is less than left, then we just return it
     */
    if (right - left <= 0) {
        return;
    }

    /** Find the mid index! */
    int mid = (left + right) / 2;

    select(left, right, mid, dimension);

    /**
     * Find the nextDimension, if the nextDimension is greater than Dim, then
     * it will be automatically set to zero
     */
    int nextDimension = dimension + 1;
    if (nextDimension >= Dim) {
        nextDimension = 0;
    }

    /**
     * Go to the left side, and the right side
     */
    buildTree(left, mid - 1, nextDimension);
    buildTree(mid + 1, right, nextDimension);
}

template <int Dim>
void KDTree<Dim>::select(int left, int right, int k, int dimension) {
    /** Check Wikipedia! */
    int pivotIndex = k;
    pivotIndex = partition(left, right, pivotIndex, dimension);

    if (k == pivotIndex) {
        return;
    } else if (k < pivotIndex) {
        select(left, pivotIndex - 1, k, dimension);
    } else {
        select(pivotIndex + 1, right, k, dimension);
    }
}

template <int Dim>
int KDTree<Dim>::partition(int left, int right, int pivotIndex, int dimension) {
    double pivotValue = points[pivotIndex][dimension];
    Point<Dim> temp = points[pivotIndex];

    swap(points[pivotIndex], points[right]);
    int storeIndex = left;

    for (int i = left; i < right; i++) {
        if (points[i][dimension] < pivotValue) {
            swap(points[storeIndex], points[i]);
            storeIndex++;

        } else if (points[i][dimension] == pivotValue) {
            /** The case that needs to fix tie-breaking issue! */
            if (points[i] < temp) {
                swap(points[storeIndex], points[i]);
                storeIndex++;
            }
        }
    }

    swap(points[right], points[storeIndex]);

    return storeIndex;
}

template <int Dim>
void KDTree<Dim>::swap(Point<Dim>& first, Point<Dim>& second) {
    if (first == second) {
        return;
    }

    Point<Dim> temp = first;
    first = second;
    second = temp;
}


template <int Dim>
Point<Dim> KDTree<Dim>::findNearestNeighbor(const Point<Dim>& query) const
{
    int right = points.size() - 1;
    Point<Dim> closest;
    bool searching = true;

    return findLeaf(query, closest, 0, right, 0, searching);
}

template <int Dim>
Point<Dim> KDTree<Dim>::findLeaf(const Point<Dim>& query,
                                Point<Dim> currClosest, int left, int right,
                                int dimension, bool& searching) const {
    /**
     * Base case when the search reaches the leaf, then we are supposed to
     * put the flag of the first search to false, so that the later reach
     * to the leaves will not automatically set the leaves as the nearest
     * neighbor.
     */
    if (right - left < 1) {
        if (searching == true) {
            searching = false;
            return points[right];
        }

        if (shouldReplace(query, currClosest, points[left])) {
            return points[left];
        } else {
            return currClosest;
        }
    }

    /** Find the middle value */
    int mid = (left + right) / 2;

    /** Find the next dimension */
    int nextDimension = dimension + 1;
    if (nextDimension == Dim) {
        nextDimension = 0;
    }

    /**
     * If the middle point is smaller than query, then goes to the right
     */
    if (smallerDimVal(points[mid], query, dimension)) {
        currClosest = findLeaf(query, currClosest, mid + 1, right,
                                                    nextDimension, searching);

        if (shouldReplace(query, currClosest, points[mid])) {
            currClosest = points[mid];
        }

        if (pow(points[mid][dimension] - query[dimension], 2) <=
                                            findDistance(currClosest, query)) {
            currClosest = findLeaf(query, currClosest, left, mid - 1,
                                                    nextDimension, searching);
        }

    } else {
        currClosest = findLeaf(query, currClosest, left, mid - 1,
                                                    nextDimension, searching);

        if (shouldReplace(query, currClosest, points[mid])) {
            currClosest = points[mid];
        }

        if (pow(points[mid][dimension] - query[dimension], 2) <=
                                            findDistance(currClosest, query)) {
            currClosest = findLeaf(query, currClosest, mid + 1, right,
                                                    nextDimension, searching);
        }
    }

    return currClosest;
}
