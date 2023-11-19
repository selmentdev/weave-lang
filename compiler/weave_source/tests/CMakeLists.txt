add_executable(weave_source_tests
 "SourceCursor.cxx"
 "SourceText.cxx"
)

target_link_libraries(weave_source_tests PUBLIC weave_source)
target_link_libraries(weave_source_tests PUBLIC thirdparty_catch2)

WEAVE_CXX_FORTIFY_CODE(weave_source_tests)

add_test(
    NAME        weave_source_tests
    COMMAND     weave_source_tests
)
