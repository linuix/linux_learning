project(signal)

set(LIBRARY_OUTPUT_PATH ../out/${PROJECT_NAME}/lib)
set(EXECUTABLE_OUTPUT_PATH ../out/${PROJECT_NAME}/execute)

include_directories(./src)

add_executable(signal src/signal_test.cpp)

