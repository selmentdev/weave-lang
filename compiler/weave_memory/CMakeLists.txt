add_library(weave_memory STATIC
 "cxx/LinearAllocator.cxx"
 "cxx/PageAllocator.cxx"
)

target_include_directories(weave_memory PUBLIC include)

target_link_libraries(weave_memory PUBLIC weave_bugcheck)
target_link_libraries(weave_memory PUBLIC weave_bitwise)

WEAVE_CXX_FORTIFY_CODE(weave_memory)
