//
// Created by benjamin on 15.08.24.
//

#ifndef JUMP_SOLVER_H
#define JUMP_SOLVER_H

#include <deque>
#include "board.h"

Board importInitialBoard(const std::string& inputFile) {
    Board initialBoard(inputFile);
    return initialBoard;
}

bool boardAlreadyInQueue(const Board& board, const std::deque<BoardStatus>& queue) {
    int targetTokenCount = board.countTokens(); // Count the number of tokens on the board

    // Iterate from back to front
    for (auto it = queue.rbegin(); it != queue.rend(); ++it) {
        int currentTokenCount = it->board.countTokens(); // Count the tokens in the current BoardStatus

        // Break if a BoardStatus has more tokens than the target board
        if (currentTokenCount > targetTokenCount) {
            break;
        }

        // Check if the boards are equivalent
        if (it->board == board) {
            return true; // The board is already in the queue
        }
    }

    return false; // The board is not in the queue
}

/**
 * Pops the first element of the queue and adds all possible next statuses at the back of the queue.
 * @param toProcess
 */
void processNextBoardStatus(std::deque<BoardStatus>& toProcess) {
    BoardStatus currentBoardStatus = toProcess.front();
    Board currentBoard = currentBoardStatus.board;
    toProcess.pop_front();
    std::vector<Turn> possible_turns = currentBoard.getAllPossibleTurns();
    for (Turn current_turn: possible_turns) {
        BoardStatus newBoardStatus(currentBoardStatus);
        newBoardStatus.applyTurn(current_turn);
        if (newBoardStatus.board.countTokens() == 1) {
            newBoardStatus.print();
            // finished calculation, clear queue
            while (!toProcess.empty()) {
                toProcess.pop_front();
            }
        } else {
            if (!boardAlreadyInQueue(newBoardStatus.board, toProcess)) {
                toProcess.push_back(newBoardStatus);
            }
        }
    }
}

bool findSolution(const Board& initialBoard) {
    std::deque<BoardStatus> toProcess;
    toProcess.emplace_back(initialBoard);
    int iterations = 0;
    while (!toProcess.empty()) {
        processNextBoardStatus(toProcess);
        ++iterations;
        if (iterations % 100 == 0) {
            toProcess.back().print();
        }

    }
    return true;
}

#endif //JUMP_SOLVER_H