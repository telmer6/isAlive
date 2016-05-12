cmake_minimum_required(VERSION 3.5)
project(IsAlive)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES src/main.cpp src/emailMessage.cpp)
add_executable(IsAlive ${SOURCE_FILES})