add_library(weave_platform STATIC)

target_include_directories(weave_platform PUBLIC include)

WEAVE_CXX_FORTIFY_CODE(weave_platform)

add_subdirectory(cxx)
