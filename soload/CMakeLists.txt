project(soLoad)

set(LIBRARY_OUTPUT_PATH ../out/${PROJECT_NAME}/lib)
set(EXECUTABLE_OUTPUT_PATH ../out/${PROJECT_NAME}/execute)

add_executable(${PROJECT_NAME} src/soLoad.cpp)

target_link_libraries(${PROJECT_NAME} dl)