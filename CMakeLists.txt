cmake_minimum_required(VERSION 3.10)
project(fastdds_demo)

set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/cmake")

add_subdirectory(example_simple)
add_subdirectory(example_shared_men)