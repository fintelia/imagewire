cmake_minimum_required(VERSION 2.8)

PROJECT(imagewire CXX)
SET(CMAKE_CXX_FLAGS "-std=c++14 -O3 -Wall -ggdb ")

link_libraries(glfw GL GLEW)
ADD_EXECUTABLE(imagewire main.cpp glhelper.cpp)

