project(signal)

set(LIBRARY_OUTPUT_PATH ./out/lib)
set(EXECUTABLE_OUTPUT_PATH ./out/execute)

include_directories(./src)

add_executable(signal src/signal_test.cpp)

