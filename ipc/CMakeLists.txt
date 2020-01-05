cmake_minimum_required(VERSION 3.9.2)
project(linux_learning)

set(CMAKE_CXX_STANDARD 14)

set(LIBRARY_OUTPUT_PATH ./build/lib)
set(EXECUTABLE_OUTPUT_PATH ./build/execute)

add_library(static SHARED src/static_lib.cpp)
include_directories(./src)

add_executable(main src/main.cpp)

link_directories(main ./build/lib)

target_link_libraries(main static)

