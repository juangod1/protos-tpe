cmake_minimum_required(VERSION 3.12)
project(MediaTypes)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    buffer.c
    buffer.h
    mediaTypes.c
    mediaTypes.h
    mediaTypesTest.c
    mediaTypesTest.h
    testLib.c
    testLib.h)

add_executable(MediaTypes ${SOURCE_FILES})