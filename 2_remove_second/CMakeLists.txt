cmake_minimum_required(VERSION 3.20)
project(remove_second LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(remove_second_test remove_second_test.cpp)
