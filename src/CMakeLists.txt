#
# Include directories
#
include_directories (${chashmap_SOURCE_DIR}/include)
include_directories (${LIBBSTRING_INCLUDE_DIR})
include_directories (${LIBCDBG_INCLUDE_DIR})
include_directories (${LIBCDARRAY_INCLUDE_DIR})

#
# We want to compile to a library and link with bstring submodule
#
add_library (chashmap hashmap.c)
target_link_libraries (
  chashmap 
  ${LIBBSTRING_LIBRARIES} 
  ${LIBCDBG_LIBRARIES} 
  ${LIBCDARRAY_LIBRARIES}
)

# 
# Library install target
#
install (TARGETS chashmap DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)