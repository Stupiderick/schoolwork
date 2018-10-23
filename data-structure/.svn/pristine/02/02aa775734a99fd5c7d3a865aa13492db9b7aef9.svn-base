#include "maze.h"

void SquareMaze::makeMaze(int width, int height) {
    /** If the width and height are less than zero, return directly! */
    if (width <= 1 || height <= 1) {
        return;
    }

    /** Clear the used Maze. */
    if (!rightWall.empty() || !downWall.empty()) {
        clearMaze();
    }

    theWidth = width;
    theHeight = height;

    /** Set up all the walls to true. */
    for (int i = 0; i < theHeight; i++) {
        for (int j = 0; j < theWidth; j++) {
            leftWall.push_back(true);
            downWall.push_back(true);
            rightWall.push_back(true);
            upWall.push_back(true);
        }
    }

    int countWallsDelete = 0;
    srand(time(NULL));
    breakWalls.addelements(theWidth * theHeight);

    /** Stop when the max of the walls deleted reaches. */
    while (countWallsDelete < theWidth * theHeight - 1) {
        int block = rand() % (theWidth * theHeight - 1);
        bool deleteRightWall = deleteWall();
        bool deleteDownWall = deleteWall();

        /** Check whether we should delete the right wall. */
        if (deleteRightWall && ((block + 1) % theWidth) != 0) {
            if(breakWalls.find(block)
                                    != breakWalls.find(block + 1)) {
                setWall(block % theWidth, block / theWidth, 0, false);
                breakWalls.setunion(block, block + 1);
                countWallsDelete++;
            }
        }

        /** Check whether we should delete the down wall. */
        if (deleteDownWall && (theWidth * theHeight - block) > theWidth) {
            if (breakWalls.find(block)
                            != breakWalls.find(block + theWidth)) {
                setWall(block % theWidth, block / theWidth, 1, false);
                breakWalls.setunion(block, block + theWidth);
                countWallsDelete++;
            }
        }
    }
}

bool SquareMaze::canTravel(int x, int y, int dir) const {
    if (x >= theWidth || y >= theHeight) {
        return false;
    }

    /** Go right, x+ */
    if (dir == 0) {
        return !(rightWall[y * theWidth + x]);
    /** Go down, y+ */
    } else if (dir == 1) {
        return !(downWall[y * theWidth + x]);
    /** Go left, x- */
    } else if (dir == 2) {
        return !(leftWall[y * theWidth + x]);
    /** Go up, y- */
    } else if (dir == 3) {
        return !(upWall[y * theWidth + x]);
    }

    return false;
}

void SquareMaze::setWall(int x, int y, int dir, bool exists) {
    if (x >= theWidth || y >= theHeight) {
        return;
    }

    /** Define the right wall. */
    if (dir == 0) {
        rightWall[y * theWidth + x] = exists;
        leftWall[y * theWidth + x + 1] = exists;
    /** Define the down wall. */
    } else if (dir == 1) {
        downWall[y * theWidth + x] = exists;
        upWall[y * theWidth + x + theWidth] = exists;
    }
}

vector<int> SquareMaze::solveMaze() {
    vector<int> ret;

    /**
     * If the bt_step is not empty, we can directly use it without calculating
     *  it again.
     */
    if (bt_step.empty()) {
        int bottomCheck = 0;
        int step = 1;
        int block = 0;
        std::pair<int, int> initPair = std::make_pair(-1, 1);

        for (int i = 0; i < theWidth * theHeight; i++) {
            bt_step.push_back(initPair);
        }

        /**
         * The logic of this part is:
         *  1. If the value of bottomCheck reaches the width, then we have
         *     gotten all the available paths to the bottom, then we all set!
         *  2. Inside the while loop, if we can travel to a specific direction,
         *     then we will check whether we have checked that direction. Let's
         *     say if the backtrack of the NEXT block is pointing to the
         *     CURRENT block, then we won't go to that block; if the backtrack
         *     of the CURRENT block is pointing to the NEXT block, we won't
         *     check it either.
         *  3. If we can go to the next block, then we will check whether it is
         *     the bottom block. Increment bottomCheck, if it is.
         *  4. If there is no available direction that we haven't check yet,
         *     then we go back according to the backtrack of the CURRENT block.
         */
        while (bottomCheck != theWidth) {
            /** Check the NEW block on the right! */
            if (canTravel(block % theWidth, block / theWidth, 0)) {
                if (bt_step[block + 1].first != 2
                                                && bt_step[block].first != 0) {
                    block = block + 1;
                    bt_step[block].first = 2;
                    step++;
                    bt_step[block].second = step;

                    if (block >= (theHeight - 1) * theWidth) {
                        bottomCheck++;
                    }
                    continue;
                }
            }

            /** Check the NEW block downward! */
            if (canTravel(block % theWidth, block / theWidth, 1)) {
                if (bt_step[block + theWidth].first != 3
                                                && bt_step[block].first != 1) {
                    block = block + theWidth;
                    bt_step[block].first = 3;
                    step++;
                    bt_step[block].second = step;

                    if (block >= (theHeight - 1) * theWidth) {
                        bottomCheck++;
                    }
                    continue;
                }
            }

            /** Check the NEW block on the left! */
            if (canTravel(block % theWidth, block / theWidth, 2)) {
                if (bt_step[block - 1].first != 0
                                                && bt_step[block].first != 2) {
                    block = block - 1;
                    bt_step[block].first = 0;
                    step++;
                    bt_step[block].second = step;

                    if (block >= (theHeight - 1) * theWidth) {
                        bottomCheck++;
                    }
                    continue;
                }
            }

            /** Check the NEW block upward! */
            if (canTravel(block % theWidth, block / theWidth, 3)) {
                if (bt_step[block - theWidth].first != 1
                                                && bt_step[block].first != 3) {
                    block = block - theWidth;
                    bt_step[block].first = 1;
                    step++;
                    bt_step[block].second = step;

                    if (block >= (theHeight - 1) * theWidth) {
                        bottomCheck++;
                    }
                    continue;
                }
            }

            /** Going beck if there is no NEW block around the CURRENT block! */
            if (bt_step[block].first == 0) {
                block += 1;
            } else if (bt_step[block].first == 1) {
                block += theWidth;
            } else if (bt_step[block].first == 2) {
                block -= 1;
            } else {
                block -= theWidth;
            }
            step--;
        }
    }

    /**
     * Find the longest the path from the original point to the bottom by
     *  comparing bt_step[i].second (storing the distance to the original point)
     *  at the bottom.
     */
    int retrack = (theWidth - 1) * theHeight;
    for (int i = (theWidth - 1) * theHeight; i < theWidth * theHeight; i++) {
        if (bt_step[i].second > bt_step[retrack].second) {
            retrack = i;
        }
    }

    /** Push the direction to the back of ret vector. */
    while (bt_step[retrack].second != 1) {
        if (bt_step[retrack].first == 0) {
            retrack += 1;
            ret.push_back(2);
        } else if (bt_step[retrack].first == 1) {
            retrack += theWidth;
            ret.push_back(3);
        } else if (bt_step[retrack].first == 2) {
            retrack -= 1;
            ret.push_back(0);
        } else {
            retrack -= theWidth;
            ret.push_back(1);
        }
    }

    /**
     * Reverse ret, since it was ordered from the bottom to the original point
     *  above.
     */
    std::reverse(ret.begin(), ret.end());

    return ret;
}

PNG * SquareMaze::drawMaze() const {
    PNG * ret = new PNG(theWidth * 10 + 1, theHeight * 10 + 1);

    /** Blacken the topmost wall of the maze. */
    for (int i = 10; i < theWidth * 10 + 1; i++) {
        (*ret)(i, 0)->red = 0;
        (*ret)(i, 0)->green = 0;
        (*ret)(i, 0)->blue = 0;
    }

    /** Blacken the leftmost wall of the maze. */
    for (int i = 0; i < theHeight * 10 + 1; i++) {
        (*ret)(0, i)->red = 0;
        (*ret)(0, i)->green = 0;
        (*ret)(0, i)->blue = 0;
    }

    for (int y = 0; y < theHeight; y++) {
        for (int x = 0; x < theWidth; x++) {
            /** Blacken the right walls of every single block, if it exists. */
            if (rightWall[y * theWidth + x]) {
                for (int k = 0; k <= 10; k++) {
                    (*ret)((x + 1) * 10, y * 10 + k)->red = 0;
                    (*ret)((x + 1) * 10, y * 10 + k)->green = 0;
                    (*ret)((x + 1) * 10, y * 10 + k)->blue = 0;
                }
            }

            /** Blacken the down walls of every single block, if it exists. */
            if (downWall[y * theWidth + x]) {
                for (int k = 0; k <= 10; k++) {
                    (*ret)(x * 10 + k, (y + 1) * 10)->red = 0;
                    (*ret)(x * 10 + k, (y + 1) * 10)->green = 0;
                    (*ret)(x * 10 + k, (y + 1) * 10)->blue = 0;
                }
            }
        }
    }

    return ret;
}

PNG * SquareMaze::drawMazeWithSolution() {
    vector<int> path = solveMaze();
    PNG * ret = drawMaze();

    int coordX = 5; // The x coordinate of the path.
    int coordY = 5; // The y coordinate of the path.

    /** Drawing the path in red. */
    for (unsigned i = 0; i < path.size(); i++) {
        if (path[i] == 0) {
            for (int k = 0; k <= 10; k++) {
                (*ret)(coordX + k, coordY)->red = 255;
                (*ret)(coordX + k, coordY)->green = 0;
                (*ret)(coordX + k, coordY)->blue = 0;
            }
            coordX += 10;
        } else if (path[i] == 1) {
            for (int k = 0; k <= 10; k++) {
                (*ret)(coordX, coordY + k)->red = 255;
                (*ret)(coordX, coordY + k)->green = 0;
                (*ret)(coordX, coordY + k)->blue = 0;
            }
            coordY += 10;
        } else if (path[i] == 2) {
            for (int k = 0; k <= 10; k++) {
                (*ret)(coordX - k, coordY)->red = 255;
                (*ret)(coordX - k, coordY)->green = 0;
                (*ret)(coordX - k, coordY)->blue = 0;
            }
            coordX -= 10;
        } else {
            for (int k = 0; k <= 10; k++) {
                (*ret)(coordX, coordY - k)->red = 255;
                (*ret)(coordX, coordY - k)->green = 0;
                (*ret)(coordX, coordY - k)->blue = 0;
            }
            coordY -= 10;
        }
    }

    /** Taking care of the exit. */
    for (int k = 1; k <= 9; k++) {
        (*ret)(coordX - 5 + k, coordY + 5)->red = 255;
        (*ret)(coordX - 5 + k, coordY + 5)->green = 255;
        (*ret)(coordX - 5 + k, coordY + 5)->blue = 255;
    }

    return ret;
}
