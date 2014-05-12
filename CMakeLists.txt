#
# Simple debug CMake settings
#
cmake_minimum_required (VERSION 2.8.12)

#
# cdebug project yo!
#
project (cdebug)

#
# Include our cmake-module submodule to help locate libraries :)
#
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/cmake-modules)

# 
# Find the bstring package
#
find_package(Bstring REQUIRED)

add_subdirectory (src)
add_subdirectory (test)