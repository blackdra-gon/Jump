//
// Created by benjamin on 14.08.24.
//

#include <catch2/catch_test_macros.hpp>
#include "solver.h"
#include "board.h"

/*TEST_CASE("One Step Simple Board") {
    Board testBoard = importInitialBoard("../testData/test_board.csv");
    std::deque<BoardStatus> toProcess;
    toProcess.emplace_back(testBoard);
    processNextBoardStatus(toProcess);
    // If test fails, check if the boards are computed in another order
    CHECK(toProcess[0].board == importInitialBoard("../testData/test_board_step_1_1.csv"));
    CHECK(toProcess[1].board == importInitialBoard("../testData/test_board_step_1_2.csv"));
}*/

TEST_CASE("Test Simple Board") {
    Board testBoard = importBoardFromCsv("../testData/test_board.csv");
    bool found_solution = findSolution(testBoard);
    CHECK(found_solution);
}

/*TEST_CASE("Duplicate Detection") {
    Board testBoard = importInitialBoard("../testData/test_duplicate_detection.csv");
    std::deque<BoardStatus> toProcess;
    toProcess.emplace_back(testBoard);
    processNextBoardStatus(toProcess);
    processNextBoardStatus(toProcess);
    processNextBoardStatus(toProcess);
    CHECK(toProcess.size() == 1);
    CHECK(toProcess[0].board == importInitialBoard("../testData/test_duplicate_detection_result.csv"));
}*/

TEST_CASE("Detect Equivalent Boards") {
    Board test_board = importBoardFromCsv("../testData/test_board_equivalence_base.csv");
    std::vector<Board> boards;
    boards.push_back(importBoardFromCsv("../testData/test_board_equivalence_90.csv"));
    boards.push_back(importBoardFromCsv("../testData/test_board_equivalence_180.csv"));
    boards.push_back(importBoardFromCsv("../testData/test_board_equivalence_270.csv"));
    boards.push_back(importBoardFromCsv("../testData/test_board_equivalence_horizontal.csv"));
    boards.push_back(importBoardFromCsv("../testData/test_board_equivalence_vertical.csv"));
    boards.push_back(importBoardFromCsv("../testData/test_board_equivalence_diagonal_1.csv"));
    boards.push_back(importBoardFromCsv("../testData/test_board_equivalence_diagonal_2.csv"));
    CHECK(test_board.is_equivalent(test_board));
    for (auto &other_board: boards) {
        CHECK(!(test_board == other_board));
    }
    for (auto &other_board: boards) {
        CHECK(test_board.is_equivalent(other_board));
    }
}

TEST_CASE("Board Compression") {
    Board standardBoard("../board.csv");
    CompressedBoard compressedStandardBoard =  standardBoard.compressedBoard();
    CHECK(compressedStandardBoard == 0b111111111111111111111101111111111111111111111);
    Board fromCompressed(0b111111111111111111111101111111111111111111111);
    CHECK(standardBoard == fromCompressed);

}

