#
# Include local headers and bstring headers
#
include_directories (${chashmap_SOURCE_DIR}/include)

#
# We want to compile to as an executable and link with our debug library
#
add_executable (hashmap_test hashmap_test.c)
target_link_libraries (hashmap_test hashmap)