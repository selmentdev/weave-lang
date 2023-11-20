add_library(weave_commandline STATIC)

target_include_directories(weave_commandline PUBLIC include)

target_link_libraries(weave_commandline PUBLIC weave_bugcheck)

WEAVE_CXX_FORTIFY_CODE(weave_commandline)

add_subdirectory(cxx)
