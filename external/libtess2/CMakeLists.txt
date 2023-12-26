cmake_minimum_required(VERSION 3.19)

project(libtess2)

file(GLOB LIBTESS2_SOURCES Source/*.c)

add_library(libtess2 STATIC ${LIBTESS2_SOURCES})
target_include_directories(libtess2 PUBLIC Include Source)
