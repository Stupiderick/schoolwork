#ifndef MAZE_H
#define MAZE_H

#include <vector>
#include <iostream>
#include "png.h"
#include "dsets.h"

using namespace std;

class SquareMaze {
public:
    /**
     * Makes a new SquareMaze of the given height and width.
     * @param the width of the new SquareMaze (number of the cells)
     * @param the height of the new SquareMaze (number of the cells)
     */
    void makeMaze(int width, int height);

    /**
     * This uses your representation of the maze to determine whether it is
     *  possible to travel in the given direction from the square at coordinate
     *  (x, y).
     * @param the x coordinate of the current cell
     * @param the y coordinate of the current cell
     * @param the desired direction to move from the current cell
     * @return whether you can travel in the specified direction
     */
    bool canTravel(int x, int y, int dir) const;

    /**
     * Sets whether or not the specified wall exists
     * @param the x coordinate of the current cell
     * @param the y coordinate of the current cell
     * @param either 0 (right) or 1 (down), which specifies which wall to set
     * @param true if setting the wall to exist, false otherwise
     */
    void setWall(int x, int y, int dir, bool exists);

    /**
     * Solves this SquareMaze
     * @return a vector of directions taken to solve the maze
     */
    vector<int> solveMaze();

    /**
     * Draw the maze without solution
     * @return a PNG of the unsolved SquareMaze
     */
    PNG * drawMaze() const;

    /**
     * This function calls drawMaze, then solveMaze; it modifies the PNG from
     *  drawMaze to show the solution vector and the exit.
     * @return a PNG of the solved SquareMaze
     */
    PNG * drawMazeWithSolution();

private:
    /** Randomly determine whether we delete walls or not, half-and-half! */
    bool deleteWall() {
        int num = rand() % 2;
        return num;
    }

    /** clear the maze info, if it has existed. */
    void clearMaze() {
        theWidth = 0;
        theHeight = 0;
        rightWall.clear();
        downWall.clear();
        leftWall.clear();
        upWall.clear();
        bt_step.clear();
    }

    int theWidth;
    int theHeight;
    vector<bool> leftWall;  // Store whether there is a left wall.
    vector<bool> rightWall; // Store whether there is a right wall.
    vector<bool> upWall;    // Store whether there is a up wall.
    vector<bool> downWall;  // Store whether there is a down wall.

    /**
     * Store two info of every single block:
     * <the direction to the prev block, the distance to the original point.
     */
    vector<std::pair<int, int>> bt_step;

    /** Storing the relations between two neighbor blocks. */
    DisjointSets breakWalls;
};


#endif
