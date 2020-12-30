cmake_minimum_required(VERSION 3.17)
project(HW3)

set(CMAKE_CXX_STANDARD 11)

add_executable(HW3 main.cpp Code_Skeleton/Part1/Semaphore.cpp Code_Skeleton/Part1/PCQueue.cpp)


SET(GCC_COVERAGE_COMPILE_FLAGS "-pthread")
add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})
SET(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}")
SET(CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}")
