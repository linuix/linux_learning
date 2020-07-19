cmake_minimum_required(VERSION 3.16)

project(elfparse)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(LIBRARY_OUTPUT_PATH ../out/${PROJECT_NAME}/lib)
set(EXECUTABLE_OUTPUT_PATH ../out/${PROJECT_NAME}/execute)

add_library(elftest SHARED elf/src/elf_test.cpp)


include_directories(../comm/include/)
link_directories(elf/file)

add_executable(${PROJECT_NAME} elf/src/elf_pars.cpp)

target_link_libraries(${PROJECT_NAME} elftest)
