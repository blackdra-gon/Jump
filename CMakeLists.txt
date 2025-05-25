cmake_minimum_required(VERSION 3.28)
project(Jump)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_BUILD_TYPE Release)

find_package(Catch2 CONFIG REQUIRED)


add_executable(Jump main.cpp
        board.cpp)
add_executable(Tests test.cpp board.cpp)

target_link_libraries(Tests PRIVATE Catch2::Catch2 Catch2::Catch2WithMain)
