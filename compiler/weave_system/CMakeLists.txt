add_library(weave_system STATIC)

target_include_directories(weave_system PUBLIC include)

target_link_libraries(weave_system PUBLIC weave_platform)
target_link_libraries(weave_system PUBLIC weave_bugcheck)
target_link_libraries(weave_system PUBLIC thirdparty_libfmt)

WEAVE_CXX_FORTIFY_CODE(weave_system)

add_subdirectory(cxx)
#add_subdirectory(tests)
