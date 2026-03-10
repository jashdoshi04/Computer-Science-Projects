/**
 * @file testsquaremaze.cpp
 * Performs basic tests of SquareMaze.
 * @date April 2007
 * @author Jonathan Ray
 */

#include <iostream>
#include "dsets.h"
#include "maze.h"
#include "cs225/PNG.h"

int main()
{
    SquareMaze m;
    m.makeMaze(50, 50);
    std::cout << "makeMaze complete" << std::endl;

    cs225::PNG *unsolved = m.drawMaze(0);
    unsolved->writeToFile("unsolved-actual.png");
    delete unsolved;
    std::cout << "drawMaze complete" << std::endl;

    std::vector<Direction> sol = m.solveMaze(0);
    std::cout << "solveMaze complete" << std::endl;

    cs225::PNG *solved = m.drawMazeWithSolution(0);
    solved->writeToFile("solved-actual.png");
    delete solved;
    std::cout << "drawMazeWithSolution complete" << std::endl;

    return 0;
}