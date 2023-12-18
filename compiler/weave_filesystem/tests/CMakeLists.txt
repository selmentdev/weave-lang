add_executable(weave_fs_tests
    "Path.cxx"
)

target_link_libraries(weave_fs_tests PUBLIC weave_filesystem)
target_link_libraries(weave_fs_tests PUBLIC thirdparty_catch2)
weave_cxx_fortify_code(weave_fs_tests)

add_test(
    NAME weave_fs_tests
    COMMAND weave_fs_tests
)
