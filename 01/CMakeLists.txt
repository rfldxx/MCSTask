cmake_minimum_required (VERSION 3.8)
 
set(CMAKE_CXX_STANDARD 17)
set(Boost_USE_STATIC_LIBS ON)

project(MyProject)
add_executable(json2dot json2dot.cpp)
