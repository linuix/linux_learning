cmake_minimum_required(VERSION 3.9.2)
project(hookmain)

set(CMAKE_CXX_STANDARD 14)

set(LIBRARY_OUTPUT_PATH ../out/${PROJECT_NAME}/lib)
set(EXECUTABLE_OUTPUT_PATH ../out/${PROJECT_NAME}/execute)

add_library(hook SHARED src/hook.c)

add_executable(hookmian src/main.c)

target_link_libraries(hookmian 
                        dl
                        -Wl,-rpath ./../lib
                        )