cmake_minimum_required(VERSION 3.14)
project(FTMR)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(FTMR_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)

enable_testing()

add_subdirectory(src)
add_subdirectory(test)