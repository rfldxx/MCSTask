cmake_minimum_required (VERSION 3.8)
 
set(CMAKE_CXX_STANDARD 17)

project(MyProject)

find_package(Boost 1.40 COMPONENTS program_options REQUIRED)
set(Boost_USE_STATIC_LIBS ON)

add_executable(generate generate.cpp)

target_link_libraries(generate PRIVATE Boost::program_options)

target_link_libraries(generate PRIVATE -static-libgcc -static-libstdc++)
