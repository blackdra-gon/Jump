//
// Created by benjamin on 18.07.24.
//

#include "board.h"

std::vector<std::vector<bool>> Board::blockedFields;

int Board::fieldSize;

BoardStatus::BoardStatus(const BoardStatusCompressed& compressedBoard) {
        board = Board(compressedBoard.getFields());
    }