cmake_minimum_required(VERSION 3.14)
project(berryscript)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)

add_definitions(-D DEBUG)
add_definitions("-Wall -g")

include_directories(src)
aux_source_directory(src DIR_SRCS)
add_executable(bs ${DIR_SRCS})