//
// Created by benjamin on 15.08.24.
//

#ifndef JUMP_SOLVER_H
#define JUMP_SOLVER_H

#include <set>
#include <deque>
#include <algorithm>
#include <execution>
#include <thread>
#include "board.h"

Board importBoardFromCsv(const std::string& inputFile) {
    Board initialBoard(inputFile);
    return initialBoard;
}

void boardAlreadyInQueueParallel(BoardStatus& boardStatus, std::deque<BoardStatusCompressed>& queue,
                                 int numberOfThreads, int threadID, std::atomic<bool>& result) {
    for (auto it = queue.begin() + threadID; it < queue.end(); it += numberOfThreads) {
        if (result) {
            break;
        }
        if (boardStatus.isEquivalent(*it)) {
            result = true;
            break;
        }
    }
}

bool boardAlreadyInQueue(BoardStatus& boardStatus, std::deque<BoardStatusCompressed>& queue) {
    boardStatus.storeEquivalentBoards();
    std::atomic<bool> foundEquivalentBoard(false);
    int numberOfThreads = 12;
    std::vector<std::thread> threads;
    for (int i = 0; i < numberOfThreads; ++i) {
        threads.emplace_back(boardAlreadyInQueueParallel, std::ref(boardStatus), std::ref(queue),numberOfThreads, i,
                             std::ref(foundEquivalentBoard));
    };
    for (auto &thread: threads) {
        thread.join();
    }
    return foundEquivalentBoard;
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
void processNextBoardStatus(BoardStatusCompressed& currentBoardStatus, std::set<BoardStatusCompressed>* writeQueue) {
    // Decompression:
    Board currentBoard(currentBoardStatus.getFields());
    std::vector<Turn> possible_turns = currentBoard.getAllPossibleTurns();
    for (Turn current_turn: possible_turns) {
        // here it is decompressed another time, maybe change later
        BoardStatus newBoardStatus(currentBoardStatus);
        newBoardStatus.applyTurn(current_turn);
        if (newBoardStatus.board.getNumberOfTokens() == 1 || newBoardStatus.board.getNumberOfTokens() == 2) {
            newBoardStatus.print();
            // finished calculation, clear queues
            // uncomment if you want to calculate only one solution
            /*while (!readQueue->empty()) {
                readQueue->pop_front();
            }
            while (!writeQueue->empty()) {
                writeQueue->pop_front();
            }*/
        } else {
            newBoardStatus.storeEquivalentBoards();
            bool equivalentBoardInQueue = false;
            for (auto board:newBoardStatus.equivalentBoards) {
                //BoardStatusCompressed boardStatus(board);
                if (writeQueue->contains(board)) {
                    equivalentBoardInQueue = true;
                }
            }
            if (!equivalentBoardInQueue) {
                BoardStatusCompressed newBoardStatusCompressed(newBoardStatus);
                writeQueue->insert(newBoardStatusCompressed);
            }
        }
    }
}

void printStatistics(int numberOfTokens, int numberOfBoardStatus) {
    std::cout << "Starting to process Boards with " << numberOfTokens << " tokens" << std::endl;
    std::cout << "There are " << numberOfBoardStatus << " Boards in the Queue" << std::endl;
    std::cout << std::endl;
}

bool findSolution(Board& initialBoard) {
    std::set<BoardStatusCompressed> firstQueue;
    std::set<BoardStatusCompressed> secondQueue;
    std::set<BoardStatusCompressed> *readQueue = &firstQueue;
    std::set<BoardStatusCompressed> *writeQueue = &secondQueue;
    BoardStatus initialStatus(initialBoard);
    readQueue->emplace(initialStatus);
    int iterations = 0;
    int currentNumberOfTokens = initialBoard.getNumberOfTokens();
    printStatistics(currentNumberOfTokens, 1);
    int numberOfBoardsInCurrentStage = 1;
    // Double while loop shall be finished only when the readQueue is empty after the queues are swapped
    while (!readQueue->empty()) {
        for (auto boardStatus: *readQueue) {
            processNextBoardStatus(boardStatus, writeQueue);
            ++iterations;
            if (iterations % 10000 == 0) {
                std::cout << iterations << "/" << numberOfBoardsInCurrentStage << std::endl;
                //toProcess.back().print();
            }
        }
        readQueue->clear();
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
