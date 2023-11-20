add_library(weave_bitwise STATIC)

target_include_directories(weave_bitwise PUBLIC include)

target_link_libraries(weave_bitwise PUBLIC weave_platform)

WEAVE_CXX_FORTIFY_CODE(weave_bitwise)

add_subdirectory(cxx)
#add_subdirectory(tests)
