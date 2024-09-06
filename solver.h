//
// Created by benjamin on 15.08.24.
//

#ifndef JUMP_SOLVER_H
#define JUMP_SOLVER_H

#include <deque>
#include <algorithm>
#include <execution>
#include "board.h"

Board importInitialBoard(const std::string& inputFile) {
    Board initialBoard(inputFile);
    return initialBoard;
}

bool boardAlreadyInQueue(const Board& board, const std::deque<BoardStatus>& queue) {
    return std::any_of(std::execution::par, queue.begin(), queue.end(),
                [board] (auto boardStatus) -> bool { return board.is_equivalent(boardStatus.board); });
    /*int targetTokenCount = board.getNumberOfTokens(); // Count the number of tokens on the board

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

    return false; // The board is not in the queue*/
}

/**
 * Pops the first element of the queue and adds all possible next statuses at the back of the queue.
 * @param toProcess
 */
void processNextBoardStatus(std::deque<BoardStatus>* readQueue, std::deque<BoardStatus>* writeQueue) {
    BoardStatus currentBoardStatus = readQueue->front();
    Board currentBoard = currentBoardStatus.board;
    readQueue->pop_front();
    std::vector<Turn> possible_turns = currentBoard.getAllPossibleTurns();
    for (Turn current_turn: possible_turns) {
        BoardStatus newBoardStatus(currentBoardStatus);
        newBoardStatus.applyTurn(current_turn);
        if (newBoardStatus.board.getNumberOfTokens() == 1) {
            newBoardStatus.print();
            // finished calculation, clear queues
            while (!readQueue->empty()) {
                readQueue->pop_front();
            }
            while (!writeQueue->empty()) {
                writeQueue->pop_front();
            }
        } else {
            if (!boardAlreadyInQueue(newBoardStatus.board, *writeQueue)) {
                writeQueue->push_back(newBoardStatus);
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
    std::deque<BoardStatus> firstQueue;
    std::deque<BoardStatus> secondQueue;
    std::deque<BoardStatus> *readQueue = &firstQueue;
    std::deque<BoardStatus> *writeQueue = &secondQueue;
    readQueue->emplace_back(initialBoard);
    int iterations = 0;
    int currentNumberOfTokens = initialBoard.getNumberOfTokens();
    printStatistics(currentNumberOfTokens, 1);
    int numberOfBoardsInCurrentStage = 1;
    // Double while loop shall be finished only when the readQueue is empty after the queues are swapped
    while (!readQueue->empty()) {
        while (!readQueue->empty()) {
            processNextBoardStatus(readQueue, writeQueue);
            ++iterations;
            if (iterations % 100 == 0) {
                std::cout << iterations << "/" << numberOfBoardsInCurrentStage << std::endl;
                //toProcess.back().print();
            }
        }
        // Print statistics and switch read an write queue
        currentNumberOfTokens--;
        numberOfBoardsInCurrentStage = writeQueue->size();
        printStatistics(currentNumberOfTokens, numberOfBoardsInCurrentStage);
        iterations = 0;
        std::swap(readQueue, writeQueue);
    }
    return true;
}

#endif //JUMP_SOLVER_H
