add_library(weave_numerics STATIC)

target_include_directories(weave_numerics PUBLIC include)

target_link_libraries(weave_numerics PUBLIC weave_bitwise)
target_link_libraries(weave_numerics PUBLIC weave_bugcheck)

WEAVE_CXX_FORTIFY_CODE(weave_numerics) 

add_subdirectory(cxx)
add_subdirectory(tests)
