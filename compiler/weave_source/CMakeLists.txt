add_library(weave_source STATIC)

target_include_directories(weave_source PUBLIC include)

target_link_libraries(weave_source PUBLIC weave_bugcheck)
target_link_libraries(weave_source PUBLIC weave_unicode)

WEAVE_CXX_FORTIFY_CODE(weave_source)

add_subdirectory(cxx)
add_subdirectory(tests)
