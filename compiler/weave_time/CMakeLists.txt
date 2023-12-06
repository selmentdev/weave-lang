add_library(weave_time STATIC)

target_include_directories(weave_time PUBLIC include)

target_link_libraries(weave_time PUBLIC weave_platform)
target_link_libraries(weave_time PUBLIC weave_bugcheck)

WEAVE_CXX_FORTIFY_CODE(weave_time)

add_subdirectory(cxx)
add_subdirectory(tests)
