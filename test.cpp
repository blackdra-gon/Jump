//
// Created by benjamin on 14.08.24.
//

#include <catch2/catch_test_macros.hpp>
#include "solver.h"

TEST_CASE("Test Simple Board") {
    Board testBoard = importInitialBoard("../test_board.csv");
    bool found_solution = findSolution(testBoard);
    CHECK(found_solution);
}