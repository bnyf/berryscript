cmake_minimum_required(VERSION 3.14)
project(berryscript)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_C_STANDARD 99)
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

add_definitions(-D DEBUG)
add_definitions("-Wall -g")

include_directories(src src/object)
aux_source_directory(src DIR_SRCS)
aux_source_directory(src/object DIR_SRCS_OBJECT)

add_executable(bs ${DIR_SRCS} ${DIR_SRCS_OBJECT})
