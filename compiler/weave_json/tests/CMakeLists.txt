add_executable(weave_json_tests
    SmokeTests.cxx
)

target_link_libraries(weave_json_tests PUBLIC weave_json)
target_link_libraries(weave_json_tests PUBLIC thirdparty_catch2)
WEAVE_CXX_FORTIFY_CODE(weave_json_tests)

add_test(
    NAME        weave_json_tests
    COMMAND     weave_json_tests
)
