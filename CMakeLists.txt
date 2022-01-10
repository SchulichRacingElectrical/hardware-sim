cmake_minimum_required(VERSION 3.0.0)
project(telemetry-sim VERSION 0.1.0)
set (CMAKE_CXX_STANDARD 20)

include(CTest)
enable_testing()

file(GLOB SOURCES
    source/*.h
    source/*.hpp
    source/*.cpp
)

add_executable(telemetry-sim ${SOURCES})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
