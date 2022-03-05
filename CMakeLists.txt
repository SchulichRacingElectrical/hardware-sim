cmake_minimum_required(VERSION 3.10)
project(TelemetrySim)
set(CMAKE_CXX_STANDARD 20)
# set(CMAKE_CXX_FLAGS "-std=c++1z")

include_directories(source)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/lib)
add_subdirectory(source)
add_subdirectory(test)
add_subdirectory(lib/googletest)

enable_testing()