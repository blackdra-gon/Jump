//
// Created by benjamin on 18.07.24.
//

#ifndef JUMP_BOARD_H
#define JUMP_BOARD_H

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>

enum Field {
    FREE,
    BLOCKED,
    TOKEN
};

typedef uint64_t CompressedBoard;

class Position {
public:
    int row;
    int column;

    Position(int row, int column) : row(row), column(column) {}

    bool operator==(const Position &other) const{
        return row == other.row && column == other.column;
    }
};

class Turn {
public:
    Position from;
    Position to;

    Turn(Position from, Position to) : from(from), to(to) {
        if (!isValidTurn(from, to)) {
            throw std::invalid_argument("Invalid turn: Positions must be exactly 2 fields apart in either x- or y-dimension, but not both.");
        }
    }

    void print() const {
        std::cout << "Jump from (" << from.row << ", " << from.column << ") to ("
                  << to.row << ", " << to.column << ")" << std::endl;
    }

    bool operator==(const Turn &other) const {
        return from == other.from && to == other.to;
    }


private:
    static bool isValidTurn(Position from, Position to) {
        int xDiff = std::abs(from.column - to.column);
        int yDiff = std::abs(from.row - to.row);
        return (xDiff == 2 && yDiff == 0) || (xDiff == 0 && yDiff == 2);
    }
};


class Board {
public:
    // Default constructor
    Board() : fields(1, std::vector<Field>(1, FREE)) {}

    // Constructor with size
    //Board(int size) : fields(size, std::vector<Field>(size, FREE)) {}

    // Constructor with rows, columns, and initial status
    Board(int size, Field initialValue) : fields(size, std::vector<Field>(size, initialValue)) {
        numberOfTokens = countTokens();
    }

    // Copy Constructor
    //Board(Board const &board) : fields(board.fields) {}

    /**
     * Imports the csv in the way that fields is a vector of rows.
     * Also the static blockedField variable is set.
     * One must only import fields of one type (size and blocked fields) in a run of the program
     *
     * @param filename csv-file to import
     */

    Board(const std::string& filename) {
        blockedFields = {};
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file");
        }

        std::vector<std::vector<Field>> tempFields;
        std::vector<std::vector<bool>> tempBlockedFields;
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::vector<Field> row;
            std::vector<bool> blockedFieldsRow;
            std::string cell;
            while (std::getline(ss, cell, ',')) {
                if (cell == "T") {
                    row.push_back(TOKEN);
                    blockedFieldsRow.push_back(false);
                } else if (cell == "X") {
                    row.push_back(BLOCKED);
                    blockedFieldsRow.push_back(true);
                } else if (cell == "O") {
                    row.push_back(FREE);
                    blockedFieldsRow.push_back(false);
                } else {
                    throw std::runtime_error("Invalid character in CSV file");
                }
            }
            tempBlockedFields.push_back(blockedFieldsRow);
            tempFields.push_back(row);
        }

        file.close();

        size_t rowCount = tempFields.size();
        for (const auto& row : tempFields) {
            if (row.size() != rowCount) {
                throw std::runtime_error("The board is not a square");
            }
        }
        blockedFields = tempBlockedFields;
        fields = tempFields;
        fieldSize = fields.size();
        numberOfTokens = countTokens();
    }

    /**
     * compressedBoard is decompressed to a 2D Vector. The information which fields are blocked is taken from
     * the static variable blockedFields.
     * @param compressedBoard
     */
    Board(CompressedBoard compressedBoard) {
        int tokenCount = 0;
        for (int i = 0; i < fieldSize; ++i) {
            std:std::vector<Field> row;
            for (int j = 0; j < fieldSize; ++j) {
                if (blockedFields[i][j]) {
                    row.push_back(BLOCKED);
                } else if (compressedBoard % 2 == 1) {
                    row.push_back(TOKEN);
                    compressedBoard >>= 1;
                    tokenCount++;
                } else {
                    row.push_back(FREE);
                    compressedBoard >>= 1;
                }
            }
            fields.push_back(row);
        }
        numberOfTokens = tokenCount;

    }

    /**
     * The Board is compressed to a 64bit integer in the following way:
     * The row are iterated from top to bottom, the fields inside the row from left to right.
     * Blocked fields are skipped, tokens are encoded with 1, empty fields with 0.
     * The first field of the order described above is encoded with the least significant bit of the integer.
     * @return
     */
    CompressedBoard compressedBoard() {
        CompressedBoard comBoard = 0;
        int64_t currentPosition = 1;
        for (int i = 0; i < fields.size(); ++i) {
            for (int j = 0; j < fields.size(); ++j) {
                if (fields[i][j] != BLOCKED) {
                    if (fields[i][j] == TOKEN) {
                        comBoard += currentPosition;
                    }
                    currentPosition <<= 1;
                }
            }
        }
        return comBoard;
    }

    std::vector<CompressedBoard> equivalentBoards() {
        size_t last_index = fields.size() - 1;
        std::vector<CompressedBoard> equivalentBoards;
        CompressedBoard comBoard = 0;
        int64_t currentPosition = 1;
        for (int i = 0; i < fields.size(); ++i) {
            for (int j = 0; j < fields.size(); ++j) {
                if (fields[i][j] != BLOCKED) {
                    if (fields[i][j] == TOKEN) {
                        comBoard += currentPosition;
                    }
                    currentPosition <<= 1;
                }
            }
        }
        equivalentBoards.push_back(comBoard);
        // One Turn
        comBoard = 0;
        currentPosition = 1;
        for (size_t i = 0; i < fields.size(); ++i) {
            for (size_t j = 0; j < fields[i].size(); ++j) {
                if (fields[j][last_index - i] != BLOCKED) {
                    if (fields[j][last_index - i] == TOKEN) {
                        comBoard += currentPosition;
                    }
                    currentPosition <<= 1;
                }
            }
        }
        equivalentBoards.push_back(comBoard);
        // Two Turns
        comBoard = 0;
        currentPosition = 1;
        for (size_t i = 0; i < fields.size(); ++i) {
            for (size_t j = 0; j < fields[i].size(); ++j) {
                if (fields[last_index - i][last_index - j] != BLOCKED) {
                    if (fields[last_index - i][last_index - j] == TOKEN) {
                        comBoard += currentPosition;
                    }
                    currentPosition <<= 1;
                }
            }
        }
        equivalentBoards.push_back(comBoard);
        // threeTurns:
        comBoard = 0;
        currentPosition = 1;
        for (size_t i = 0; i < fields.size(); ++i) {
            for (size_t j = 0; j < fields[i].size(); ++j) {
                if (fields[last_index - j][i] != BLOCKED) {
                    if (fields[last_index - j][i] == TOKEN) {
                        comBoard += currentPosition;
                    }
                    currentPosition <<= 1;
                }
            }
        }
        equivalentBoards.push_back(comBoard);
        // horizontal
        comBoard = 0;
        currentPosition = 1;
        for (size_t i = 0; i < fields.size(); ++i) {
            for (size_t j = 0; j < fields[i].size(); ++j) {
                if (fields[i][last_index - j] != BLOCKED) {
                    if (fields[i][last_index - j] == TOKEN) {
                        comBoard += currentPosition;
                    }
                    currentPosition <<= 1;
                }
            }
        }
        equivalentBoards.push_back(comBoard);
        // vertical
        comBoard = 0;
        currentPosition = 1;
        for (size_t i = 0; i < fields.size(); ++i) {
            for (size_t j = 0; j < fields[i].size(); ++j) {
                if (fields[last_index - i][j] != BLOCKED) {
                    if (fields[last_index - i][j] == TOKEN) {
                        comBoard += currentPosition;
                    }
                    currentPosition <<= 1;
                }
            }
        }
        equivalentBoards.push_back(comBoard);
        // diagonal
        comBoard = 0;
        currentPosition = 1;
        for (size_t i = 0; i < fields.size(); ++i) {
            for (size_t j = 0; j < fields[i].size(); ++j) {
                if (fields[j][i] != BLOCKED) {
                    if (fields[j][i] == TOKEN) {
                        comBoard += currentPosition;
                    }
                    currentPosition <<= 1;
                }
            }
        }
        equivalentBoards.push_back(comBoard);
        // secondDiagonal
        comBoard = 0;
        currentPosition = 1;
        for (size_t i = 0; i < fields.size(); ++i) {
            for (size_t j = 0; j < fields[i].size(); ++j) {
                if (fields[last_index - j][last_index - i] != BLOCKED) {
                    if (fields[last_index - j][last_index - i] == TOKEN) {
                        comBoard += currentPosition;
                    }
                    currentPosition <<= 1;
                }
            }
        }
        equivalentBoards.push_back(comBoard);
        return equivalentBoards;
    }

    void printBoard() const {
        for (const auto& row : fields) {
            for (const auto& cell : row) {
                std::cout << fieldToString(cell) << ",";
            }
            std::cout << std::endl;
        }
    }


    Field getField(int row, int col) const {
        if (row >= 0 && row < fields.size() && col >= 0 && col < fields[0].size()) {
            return fields[row][col];
        } else {
            std::cerr << "Invalid indices" << std::endl;
            return FREE; // Return default value
        }
    }

    Field getField(const Position& pos) const {
        return getField(pos.row, pos.column);
    }

    bool operator==(const Board& other) const {
        if (fields.size() != other.fields.size()) {
            return false;
        }

        for (size_t i = 0; i < fields.size(); ++i) {
            if (fields[i].size() != other.fields[i].size()) {
                return false;
            }
            for (size_t j = 0; j < fields[i].size(); ++j) {
                if (fields[i][j] != other.fields[i][j]) {
                    return false;
                }
            }
        }

        return true;
    }

    bool is_equivalent(const Board& other) const {
        // Assuming that all boards have the right size
        size_t last_index = fields.size() - 1;
        for (size_t i = 0; i < fields.size(); ++i) {
            for (size_t j = 0; j < fields[i].size(); ++j) {
                if (fields[i][j] != other.fields[i][j]) {
                    goto oneTurn;
                }
            }
        }
        return true;
        oneTurn:
        for (size_t i = 0; i < fields.size(); ++i) {
            for (size_t j = 0; j < fields[i].size(); ++j) {
                if (fields[j][last_index - i] != other.fields[i][j]) {
                    goto twoTurns;
                }
            }
        }
        return true;
        twoTurns:
        for (size_t i = 0; i < fields.size(); ++i) {
            for (size_t j = 0; j < fields[i].size(); ++j) {
                if (fields[last_index - i][last_index - j] != other.fields[i][j]) {
                    goto threeTurns;
                }
            }
        }
        return true;
        threeTurns:
        for (size_t i = 0; i < fields.size(); ++i) {
            for (size_t j = 0; j < fields[i].size(); ++j) {
                if (fields[last_index - j][i] != other.fields[i][j]) {
                    goto horizontal;
                }
            }
        }
        return true;
        horizontal:
        for (size_t i = 0; i < fields.size(); ++i) {
            for (size_t j = 0; j < fields[i].size(); ++j) {
                if (fields[i][last_index - j] != other.fields[i][j]) {
                    goto vertical;
                }
            }
        }
        return true;
        vertical:
        for (size_t i = 0; i < fields.size(); ++i) {
            for (size_t j = 0; j < fields[i].size(); ++j) {
                if (fields[last_index - i][j] != other.fields[i][j]) {
                    goto diagonal;
                }
            }
        }
        return true;
        diagonal:
        for (size_t i = 0; i < fields.size(); ++i) {
            for (size_t j = 0; j < fields[i].size(); ++j) {
                if (fields[j][i] != other.fields[i][j]) {
                    goto secondDiagonal;
                }
            }
        }
        return true;
        secondDiagonal:
        for (size_t i = 0; i < fields.size(); ++i) {
            for (size_t j = 0; j < fields[i].size(); ++j) {
                if (fields[last_index - j][last_index - i] != other.fields[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

    int countTokens() const {
        int tokenCount = 0;
        for (const auto& row : fields) {
            for (const auto& cell : row) {
                if (cell == TOKEN) {
                    ++tokenCount;
                }
            }
        }
        return tokenCount;
    }

    void applyTurn(Turn turn) {
        Position from = turn.from;
        Position to = turn.to;

        // Calculate the middle position
        int midRow = (from.row + to.row) / 2;
        int midCol = (from.column + to.column) / 2;
        Position middle(midRow, midCol);

        // Check preconditions
        if (getField(from) == TOKEN && getField(to) == FREE && getField(middle) == TOKEN) {
            setField(from, FREE);
            setField(middle, FREE);
            setField(to, TOKEN);
            numberOfTokens--;
        } else {
            std::cerr << "Turn cannot be applied due to invalid conditions." << std::endl;
        }
    }
    std::vector<Turn> getAllPossibleTurns() const {
        std::vector<Turn> possibleTurns;

        for (int row = 0; row < fields.size(); ++row) {
            for (int column = 0; column < fields[row].size(); ++column) {
                if (fields[row][column] == TOKEN) {
                    // Check possible moves in four directions
                    // Move right
                    if (isValidMove(row, column, row, column + 2)) {
                        possibleTurns.emplace_back(Position(row, column), Position(row, column + 2));
                    }
                    // Move left
                    if (isValidMove(row, column, row, column - 2)) {
                        possibleTurns.emplace_back(Position(row, column), Position(row, column - 2));
                    }
                    // Move down
                    if (isValidMove(row, column, row + 2, column)) {
                        possibleTurns.emplace_back(Position(row, column), Position(row + 2, column));
                    }
                    // Move up
                    if (isValidMove(row, column, row - 2, column)) {
                        possibleTurns.emplace_back(Position(row, column), Position(row - 2, column));
                    }
                }
            }
        }

        return possibleTurns;
    }

    int getNumberOfTokens() const {
        return numberOfTokens;
    }


    static std::vector<std::vector<bool>> blockedFields;
    static int fieldSize;
private:
    std::vector<std::vector<Field>> fields;
    int numberOfTokens;



    void setField(int row, int col, Field field) {
        if (row >= 0 && row < fields.size() && col >= 0 && col < fields[0].size()) {
            fields[row][col] = field;
        } else {
            std::cerr << "Invalid indices" << std::endl;
        }
    }

    void setField(const Position& pos, Field field) {
        setField(pos.row, pos.column, field);
    }

    bool isValidMove(int fromX, int fromY, int toX, int toY) const {
        // Check if target position is within bounds
        if (toX < 0 || toX >= fields.size() || toY < 0 || toY >= fields[0].size()) {
            return false;
        }

        // Calculate middle position
        int midX = (fromX + toX) / 2;
        int midY = (fromY + toY) / 2;

        // Check if the move is valid according to the described logic
        return (fields[fromX][fromY] == TOKEN &&
                fields[toX][toY] == FREE &&
                fields[midX][midY] == TOKEN);
    }

    std::string fieldToString(Field status) const {
        switch (status) {
            case FREE: return "O";
            case BLOCKED: return "X";
            case TOKEN: return "T";
            default: return "UNKNOWN";
        }
    }

};

class BoardStatusCompressed;


class BoardStatus {
public:
    Board board;
    std::vector<Turn> turns;
    std::vector<CompressedBoard> equivalentBoards;
    // A turn sequence is a sequence of turns, where the same token is moved.
    // Two consecutive turns t1 and t2 belong to the same sequence, iff t1.to == t2.from
    int numberOfTurnSequences;

    // Constructor to initialize the BoardStatus with a given board
    BoardStatus(const Board& board) : board(board) {
        numberOfTurnSequences = 0;
    }

    BoardStatus(const BoardStatusCompressed& compressedBoard);

    // Function to apply a turn to the board and add the turn to the vector
    void applyTurn(Turn turn) {
        adjustNumberOfTurnSequences(turn);
        board.applyTurn(turn);
        turns.push_back(turn);
    }

    void print() const {

        std::cout << "Turns applied:" << std::endl;
        for (const auto& turn : turns) {
            turn.print(); // Using the print function of Turn
        }
        std::cout << "Number of turn sequences: " << numberOfTurnSequences << std::endl;
        std::cout << "Board:" << std::endl;
        board.printBoard();
    }

    void storeEquivalentBoards() {
        equivalentBoards = board.equivalentBoards();
    }

    /**
    Should only be called, when equivalent Board is filled with all (up to) 8 equivalent Boards
     */
    bool isEquivalent(BoardStatusCompressed &compressedBoard);

private:
    void adjustNumberOfTurnSequences(Turn nextTurn) {
        if (turns.empty() || !(turns.back().to == nextTurn.from)) {
            ++numberOfTurnSequences;
        } 
    }
};

class BoardStatusCompressed {
    CompressedBoard fields;
    

public:
    std::vector<Turn> turns;
    int numberOfTurnSequences;

    CompressedBoard getFields() const {
        return fields;
    }

    BoardStatusCompressed(BoardStatus &boardStatus) {
        fields = boardStatus.board.compressedBoard();
        turns = boardStatus.turns;   // std::move??
        numberOfTurnSequences = boardStatus.numberOfTurnSequences;
    }

    BoardStatusCompressed(CompressedBoard comBoard) {
        fields = comBoard;
    }

    bool operator<(const BoardStatusCompressed& other) const {
        return fields < other.fields;
    }
};


#endif //JUMP_BOARD_H
