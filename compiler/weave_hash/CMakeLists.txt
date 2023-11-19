add_library(weave_hash STATIC
 cxx/sha256.cxx
)

target_include_directories(weave_hash PUBLIC include)

target_link_libraries(weave_hash PUBLIC weave_bitwise)
target_link_libraries(weave_hash PUBLIC thirdparty_libfmt)
WEAVE_CXX_FORTIFY_CODE(weave_hash)

add_subdirectory(tests)
