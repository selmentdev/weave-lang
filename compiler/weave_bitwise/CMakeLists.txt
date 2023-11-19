add_library(weave_bitwise INTERFACE)

target_include_directories(weave_bitwise INTERFACE include)

target_link_libraries(weave_bitwise INTERFACE weave_platform)

#WEAVE_CXX_FORTIFY_CODE(weave_bitwise)

# add_subdirectory(tests)
