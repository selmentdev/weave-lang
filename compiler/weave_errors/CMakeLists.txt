add_library(weave_errors STATIC)

target_include_directories(weave_errors PUBLIC include)

WEAVE_CXX_FORTIFY_CODE(weave_errors)

add_subdirectory(cxx)
#add_subdirectory(tests)
