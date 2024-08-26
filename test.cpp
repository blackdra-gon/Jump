//
// Created by benjamin on 14.08.24.
//

#include <catch2/catch_test_macros.hpp>
#include "solver.h"

TEST_CASE("One Step Simple Board") {
    Board testBoard = importInitialBoard("../testData/test_board.csv");
    std::deque<BoardStatus> toProcess;
    toProcess.emplace_back(testBoard);
    processNextBoardStatus(toProcess);
    // If test fails, check if the boards are computed in another order
    CHECK(toProcess[0].board == importInitialBoard("../testData/test_board_step_1_1.csv"));
    CHECK(toProcess[1].board == importInitialBoard("../testData/test_board_step_1_2.csv"));
}

TEST_CASE("Test Simple Board") {
    Board testBoard = importInitialBoard("../testData/test_board.csv");
    bool found_solution = findSolution(testBoard);
    CHECK(found_solution);
}

TEST_CASE("Duplicate Detection") {
    Board testBoard = importInitialBoard("../testData/test_duplicate_detection.csv");
    std::deque<BoardStatus> toProcess;
    toProcess.emplace_back(testBoard);
    processNextBoardStatus(toProcess);
    processNextBoardStatus(toProcess);
    processNextBoardStatus(toProcess);
    CHECK(toProcess.size() == 1);
    CHECK(toProcess[0].board == importInitialBoard("../testData/test_duplicate_detection_result.csv"));
}

