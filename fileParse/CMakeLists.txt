cmake_minimum_required(VERSION 3.16)

project(elfparse)

set(CMAKE_VERBOSE_MAKEFILE ON)
set(LIBRARY_OUTPUT_PATH ../out/${PROJECT_NAME}/lib)
set(EXECUTABLE_OUTPUT_PATH ../out/${PROJECT_NAME}/execute)


include_directories(../comm/include/)
link_directories(./elf/file)

add_library(elftest SHARED elf/src/elf_test.cpp)



add_executable(${PROJECT_NAME} elf/src/elf_pars.cpp)

# target_link_libraries(${PROJECT_NAME} elftest)
target_link_libraries(${PROJECT_NAME} dl)


add_executable(encription elf/src/elf_encription.cpp)



add_executable(decription elf/src/elf_decription.cpp)


target_link_libraries(decription dl)

