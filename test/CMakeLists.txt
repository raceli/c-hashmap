#
# Include directories
#
include_directories (${chashmap_SOURCE_DIR}/include)
include_directories (${LIBBSTRING_INCLUDE_DIR})
include_directories (${LIBCDBG_INCLUDE_DIR})
include_directories (${LIBDARRAY_INCLUDE_DIR})

#
# We want to compile to as an executable and link with our debug library
#
add_executable (hashmap_test hashmap_test.c)
target_link_libraries (hashmap_test hashmap)