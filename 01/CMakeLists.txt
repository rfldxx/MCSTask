cmake_minimum_required (VERSION 3.8)
 
set(CMAKE_CXX_STANDARD 17)

project(MyProject)

find_package(Boost 1.40 COMPONENTS program_options REQUIRED)
set(Boost_USE_STATIC_LIBS ON)

add_executable(json2dot json2dot.cpp)

target_link_libraries(json2dot PRIVATE Boost::program_options)

target_link_libraries(json2dot PRIVATE -static-libgcc -static-libstdc++)
