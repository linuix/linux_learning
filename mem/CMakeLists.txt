project(mem)


set(LIBRARY_OUTPUT_PATH ../out/${PROJECT_NAME}/lib)
set(EXECUTABLE_OUTPUT_PATH ../out/${PROJECT_NAME}/execute)

include_directories(../comm/include/)

add_executable(${PROJECT_NAME} src/memTest.cpp )

