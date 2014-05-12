#
# Include local headers and bstring headers
#
include_directories (${hashmap_SOURCE_DIR}/include)
include_directories (${LIBBSTRING_INCLUDE_DIR})
include_directories (${LIBCDBG_INCLUDE_DIR})

#
# We want to compile to a library and link with bstring submodule
#
add_library (hashmap hashmap.c)
target_link_libraries (hashmap ${LIBBSTRING_LIBRARIES} ${LIBCDBG_LIBRARIES})

# 
# Library install target
#
install (TARGETS hashmap DESTINATION lib)