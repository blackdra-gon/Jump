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

enum Field {
    FREE,
    BLOCKED,
    TOKEN
};

class Position {
public:
    int x;
    int y;

    Position(int x, int y) : x(x), y(y) {}
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
        std::cout << "Jump from (" << from.x << ", " << from.y << ") to ("
                  << to.x << ", " << to.y << ")" << std::endl;
    }


private:
    static bool isValidTurn(Position from, Position to) {
        int xDiff = std::abs(from.x - to.x);
        int yDiff = std::abs(from.y - to.y);
        return (xDiff == 2 && yDiff == 0) || (xDiff == 0 && yDiff == 2);
    }
};


class Board {
public:
    // Default constructor
    Board() : fields(1, std::vector<Field>(1, FREE)) {}

    // Constructor with size
    Board(int size) : fields(size, std::vector<Field>(size, FREE)) {}

    // Constructor with rows, columns, and initial status
    Board(int size, Field initialValue) : fields(size, std::vector<Field>(size, initialValue)) {
        numberOfTokens = countTokens();
    }

    // Copy Constructor
    //Board(Board const &board) : fields(board.fields) {}

    Board(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file");
        }

        std::vector<std::vector<Field>> tempFields;
        std::string line;
        while (std::getline(file, line)) {
            std::stringstream ss(line);
            std::vector<Field> row;
            std::string cell;
            while (std::getline(ss, cell, ',')) {
                if (cell == "T") {
                    row.push_back(TOKEN);
                } else if (cell == "X") {
                    row.push_back(BLOCKED);
                } else if (cell == "O") {
                    row.push_back(FREE);
                } else {
                    throw std::runtime_error("Invalid character in CSV file");
                }
            }
            tempFields.push_back(row);
        }

        file.close();

        size_t rowCount = tempFields.size();
        for (const auto& row : tempFields) {
            if (row.size() != rowCount) {
                throw std::runtime_error("The board is not a square");
            }
        }

        fields = tempFields;
        numberOfTokens = countTokens();
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
        return getField(pos.x, pos.y);
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
        int midX = (from.x + to.x) / 2;
        int midY = (from.y + to.y) / 2;
        Position middle(midX, midY);

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

        for (int x = 0; x < fields.size(); ++x) {
            for (int y = 0; y < fields[x].size(); ++y) {
                if (fields[x][y] == TOKEN) {
                    // Check possible moves in four directions
                    // Move right
                    if (isValidMove(x, y, x, y + 2)) {
                        possibleTurns.emplace_back(Position(x, y), Position(x, y + 2));
                    }
                    // Move left
                    if (isValidMove(x, y, x, y - 2)) {
                        possibleTurns.emplace_back(Position(x, y), Position(x, y - 2));
                    }
                    // Move down
                    if (isValidMove(x, y, x + 2, y)) {
                        possibleTurns.emplace_back(Position(x, y), Position(x + 2, y));
                    }
                    // Move up
                    if (isValidMove(x, y, x - 2, y)) {
                        possibleTurns.emplace_back(Position(x, y), Position(x - 2, y));
                    }
                }
            }
        }

        return possibleTurns;
    }

    int getNumberOfTokens() const {
        return numberOfTokens;
    }



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
        setField(pos.x, pos.y, field);
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

class BoardStatus {
public:
    Board board;
    std::vector<Turn> turns;

    // Constructor to initialize the BoardStatus with a given board
    BoardStatus(const Board& board) : board(board) {}

    // Function to apply a turn to the board and add the turn to the vector
    void applyTurn(Turn turn) {
        board.applyTurn(turn);
        turns.push_back(turn);
    }

    void print() const {

        std::cout << "Turns applied:" << std::endl;
        for (const auto& turn : turns) {
            turn.print(); // Using the print function of Turn
        }
        std::cout << "Board:" << std::endl;
        board.printBoard();
    }
};


#endif //JUMP_BOARD_H
