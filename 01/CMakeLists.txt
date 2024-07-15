cmake_minimum_required (VERSION 3.8)
 
set(CMAKE_CXX_STANDARD 17)

project(MyProject)

set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.40 COMPONENTS program_options REQUIRED)

add_executable(json2dot json2dot.cpp)

target_link_libraries(json2dot PRIVATE Boost::program_options)

# find_package(Boost REQUIRED COMPONENTS program_options)
# find_package( Boost 1.30 COMPONENTS program_options REQUIRED )
# target_include_directories( myTarget PRIVATE ${Boost_INCLUDE_DIR})
# target_link_libraries( myTarget ${Boost_LIBRARIES} )




target_link_libraries(json2dot PRIVATE -static-libgcc -static-libstdc++)



# Настройка статической линковки libstdc++
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libgcc -static-libstdc++")

# Используйте стандартное CMake-правило для добавления вашего исполняемого файла
# add_executable(YourProject main.cpp)

# Включите статическую линковку (-static) для цели вашего проекта
# set_target_properties(json2dot PROPERTIES LINK_SEARCH_START_STATIC 1)
# set_target_properties(json2dot PROPERTIES LINK_SEARCH_END_STATIC 1)
