add_library(weave_bugcheck STATIC)

target_include_directories(weave_bugcheck PUBLIC include)

target_link_libraries(weave_bugcheck PUBLIC weave_platform)
target_link_libraries(weave_bugcheck PUBLIC thirdparty_libfmt)

if(LINUX)
    target_link_libraries(weave_bugcheck INTERFACE stdc++_libbacktrace)
endif()

WEAVE_CXX_FORTIFY_CODE(weave_bugcheck)

add_subdirectory(cxx)
# add_subdirectory(tests)
