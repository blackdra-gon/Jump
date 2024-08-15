//
// Created by benjamin on 15.08.24.
//

#ifndef JUMP_SOLVER_H
#define JUMP_SOLVER_H

#include <queue>
#include "board.h"

Board importInitialBoard(const std::string& inputFile) {
    Board initialBoard(inputFile);
    return initialBoard;
}

bool findSolution(Board initialBoard) {
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
    return true;
}

#endif //JUMP_SOLVER_H
