#include <iostream>
#include <queue>
#include "board.h"

int main(int argc, char** argv) {
    std::string input_file;
    if ( argc > 1 ) {
        input_file = argv[1];
    }
    Board initialBoard(input_file);
    std::cout << "Initial Board:" << std::endl;
    initialBoard.printBoard();
    /*try {
        Board board("../board.csv"); // Constructor that imports initial values from a CSV file
        std::cout << "Board:" << std::endl;
        board.printBoard();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }*/
    std::queue<BoardStatus> toProcess;
    toProcess.emplace(initialBoard);
    int iterations = 0;
    while (!toProcess.empty()) {
        ++iterations;
        BoardStatus currentBoardStatus = toProcess.front();
        Board currentBoard = currentBoardStatus.board;
        toProcess.pop();
        std::vector<Turn> possible_turns = currentBoard.getAllPossibleTurns();
        for (Turn current_turn: possible_turns) {
            BoardStatus newBoardStatus(currentBoardStatus);
            newBoardStatus.applyTurn(current_turn);
            if (newBoardStatus.board.countTokens() == 1) {
                newBoardStatus.print();
                // finished calculation, clear queue
                while (!toProcess.empty()) {
                    toProcess.pop();
                }
            } else {
                toProcess.push(newBoardStatus);
                if (iterations % 100 == 0) {
                    newBoardStatus.print();
                }
            }
        }
    }
   return 0;
}
