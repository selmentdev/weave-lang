add_executable(weave_bitwise_tests
    "CompressedInteger.cxx"
)

target_link_libraries(weave_bitwise_tests PUBLIC weave_bitwise)
target_link_libraries(weave_bitwise_tests PUBLIC thirdparty_catch2)
weave_cxx_fortify_code(weave_bitwise_tests)

add_test(
    NAME weave_bitwise_tests
    COMMAND weave_bitwise_tests
)
