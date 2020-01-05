cmake_minimum_required(VERSION 3.9.2)
project(hook)

set(CMAKE_CXX_STANDARD 14)

set(LIBRARY_OUTPUT_PATH ./build/lib)
set(EXECUTABLE_OUTPUT_PATH ./build/execute)


add_executable(hook src/main.c src/hook.c)

target_link_libraries(hook dl)