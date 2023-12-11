add_library(weave_uuid STATIC)

target_include_directories(weave_uuid PUBLIC include)

target_link_libraries(weave_uuid PUBLIC weave_hash)
target_link_libraries(weave_uuid PUBLIC weave_bugcheck)
target_link_libraries(weave_uuid PUBLIC weave_bitwise)

WEAVE_CXX_FORTIFY_CODE(weave_uuid)

add_subdirectory(cxx)
add_subdirectory(tests)
