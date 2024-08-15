#include <iostream>
#include "board.h"
#include "solver.h"



int main(int argc, char** argv) {
    // Is the Board allocated doubly because of the funtion call?
    Board initialBoard = importInitialBoard(argv[1]);
    std::cout << "Initial Board:" << std::endl;
    initialBoard.printBoard();
    findSolution(initialBoard);

   return 0;
}
