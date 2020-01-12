project(soLoad)

add_executable(soLoad src/soLoad.cpp)

target_link_libraries(soLoad dl)