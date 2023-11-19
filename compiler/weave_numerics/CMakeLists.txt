add_library(weave_numerics STATIC
 cxx/Int128.cxx
 cxx/UInt128.cxx
)

target_include_directories(weave_numerics PUBLIC include)

target_link_libraries(weave_numerics PUBLIC weave_bitwise)
target_link_libraries(weave_numerics PUBLIC weave_bugcheck)

WEAVE_CXX_FORTIFY_CODE(weave_numerics) 

add_subdirectory(tests)
