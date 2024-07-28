#include <iostream>
#include <queue>
#include "board.h"

int main() {
    Board initialBoard("../board.csv");
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
    while (!toProcess.empty()) {
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
            }
        }
    }
   return 0;
}
