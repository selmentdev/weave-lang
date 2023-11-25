add_executable(weave_unicode_tests
    Encoding.cxx
)

target_link_libraries(weave_unicode_tests PUBLIC weave_unicode)
target_link_libraries(weave_unicode_tests PUBLIC thirdparty_catch2)


WEAVE_CXX_FORTIFY_CODE(weave_unicode_tests)

add_test(
    NAME
        weave_unicode_tests
    COMMAND
        weave_unicode_tests
)
