add_library(weave_filesystem STATIC)

target_include_directories(weave_filesystem PUBLIC include)

target_link_libraries(weave_filesystem PUBLIC weave_bugcheck)

WEAVE_CXX_FORTIFY_CODE(weave_filesystem)

add_subdirectory(cxx)
