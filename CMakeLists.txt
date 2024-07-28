cmake_minimum_required(VERSION 3.28)
project(Jump)

set(CMAKE_CXX_STANDARD 17)

add_executable(Jump main.cpp
        board.cpp
        board.h)
