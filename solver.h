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
    int targetTokenCount = board.getNumberOfTokens(); // Count the number of tokens on the board

    // Iterate from back to front
    for (auto it = queue.rbegin(); it != queue.rend(); ++it) {
        int currentTokenCount = it->board.getNumberOfTokens(); // Count the tokens in the current BoardStatus

        // Break if a BoardStatus has more tokens than the target board
        if (currentTokenCount > targetTokenCount) {
            break;
        }

        // Check if the boards are equivalent
        if (it->board.is_equivalent(board)) {
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
        if (newBoardStatus.board.getNumberOfTokens() == 1) {
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

void printStatistics(int numberOfTokens, int numberOfBoardStatus) {
    std::cout << "Starting to process Boards with " << numberOfTokens << " tokens" << std::endl;
    std::cout << "There are " << numberOfBoardStatus << " Boards in the Queue" << std::endl;
    std::cout << std::endl;
}

bool findSolution(const Board& initialBoard) {
    std::deque<BoardStatus> toProcess;
    toProcess.emplace_back(initialBoard);
    int iterations = 0;
    int currentNumberOfTokens = initialBoard.getNumberOfTokens();
    printStatistics(currentNumberOfTokens, 1);
    while (!toProcess.empty()) {
        int numberOfBoardsInCurrentStage;
        if (toProcess.front().board.getNumberOfTokens() < currentNumberOfTokens) {
            currentNumberOfTokens--;
            numberOfBoardsInCurrentStage = toProcess.size();
            printStatistics(currentNumberOfTokens, numberOfBoardsInCurrentStage);
            iterations = 0;
        }
        processNextBoardStatus(toProcess);
        ++iterations;
        if (iterations % 100 == 0) {
            std::cout << iterations << "/" << numberOfBoardsInCurrentStage << std::endl;
            //toProcess.back().print();
        }

    }
    return true;
}

#endif //JUMP_SOLVER_H
