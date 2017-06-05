cmake_minimum_required(VERSION 3.6)
project(fuse)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ")

set(BUILD main)
set(SOURCE_FILES jfs.c)
add_executable(${BUILD} ${SOURCE_FILES})