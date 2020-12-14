cmake_minimum_required(VERSION 3.14)
project(berryscript)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_C_STANDARD 99)

include_directories(src)

add_executable(c_fileserver
        src/parser.cpp
        src/parser.h
        src/utils.cpp
        src/utils.h
        src/vm.cpp
        src/vm.h
        src/main.cpp)