add_library(weave_memory STATIC)

target_include_directories(weave_memory PUBLIC include)

target_link_libraries(weave_memory PUBLIC weave_bugcheck)
target_link_libraries(weave_memory PUBLIC weave_bitwise)

WEAVE_CXX_FORTIFY_CODE(weave_memory)

add_subdirectory(cxx)
