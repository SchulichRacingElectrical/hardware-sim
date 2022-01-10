cmake_minimum_required(VERSION 3.0.0)
project(TelemetrySim)
set(CMAKE_CXX_STANDARD 20)

include_directories(source)
include_directories(inc)
add_subdirectory(source)
add_subdirectory(test)
add_subdirectory(lib/googletest)

enable_testing()