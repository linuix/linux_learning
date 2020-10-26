cmake_minimum_required(VERSION 3.9.2)
project(mian)

set(CMAKE_CXX_STANDARD 14)

set(LIBRARY_OUTPUT_PATH ../out/${PROJECT_NAME}/lib)
set(EXECUTABLE_OUTPUT_PATH ../out/${PROJECT_NAME}/execute)

add_library(testlib SHARED src/static_lib.cpp)
include_directories(./src)


link_directories(main out/lib)

add_executable(main src/main.cpp)


target_link_libraries(main testlib)

