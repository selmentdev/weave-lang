add_executable(weave_uuid_tests
    "Uuid.cxx"
)

target_link_libraries(weave_uuid_tests PUBLIC weave_uuid)
target_link_libraries(weave_uuid_tests PUBLIC thirdparty_catch2)


WEAVE_CXX_FORTIFY_CODE(weave_uuid_tests)

add_test(
    NAME
        weave_uuid_tests
    COMMAND
        weave_uuid_tests
)
