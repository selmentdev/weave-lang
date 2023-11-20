add_library(weave_json STATIC)

target_include_directories(weave_json PUBLIC include)

target_link_libraries(weave_json PUBLIC weave_bugcheck)
target_link_libraries(weave_json PUBLIC weave_source)
target_link_libraries(weave_json PUBLIC weave_unicode)

WEAVE_CXX_FORTIFY_CODE(weave_json)

add_subdirectory(cxx)
add_subdirectory(tests)
