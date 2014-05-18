#
# Simple debug CMake settings
#
cmake_minimum_required (VERSION 2.8.12)

#
# chashmap project yo!
#
project (chashmap)

#
# c flags
#
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -Wall")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall")

#
# because I have cmake-modules checked out in my code directory I know that I 
# should always have a cmake-modules folder available below this project's src
# dir.
#
set(
  CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH} 
  ${PROJECT_SOURCE_DIR}/../cmake-modules
)

# 
# Find the bstring package
#
find_package(Bstring REQUIRED)
find_package(CDbg REQUIRED)
find_package(CDarray REQUIRED)

add_subdirectory (src)
add_subdirectory (test)

#
# Copy Include Directory to the Installation Prefix (i.e. /usr/local/)
#
install (
  DIRECTORY ${chashmap_SOURCE_DIR}/include
  DESTINATION ${CMAKE_INSTALL_PREFIX}
  FILES_MATCHING PATTERN "*.h"
)