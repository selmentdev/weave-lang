add_library(weave_syntax STATIC)

target_include_directories(weave_syntax PUBLIC include)

target_link_libraries(weave_syntax PUBLIC weave_bugcheck)
target_link_libraries(weave_syntax PUBLIC weave_unicode)
target_link_libraries(weave_syntax PUBLIC weave_memory)
target_link_libraries(weave_syntax PUBLIC weave_stringpool)
target_link_libraries(weave_syntax PUBLIC weave_source)
target_link_libraries(weave_syntax PUBLIC weave_hash)

WEAVE_CXX_FORTIFY_CODE(weave_syntax)

add_subdirectory(cxx)
add_subdirectory(tests)
