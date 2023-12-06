add_executable(weave_time_tests
    "DateTime.cxx"
    "DateTimeOffset.cxx"
    "Duration.cxx"
)

target_link_libraries(weave_time_tests PUBLIC weave_time)
target_link_libraries(weave_time_tests PUBLIC thirdparty_catch2)


WEAVE_CXX_FORTIFY_CODE(weave_time_tests)

add_test(
    NAME
        weave_time_tests
    COMMAND
        weave_time_tests
)
