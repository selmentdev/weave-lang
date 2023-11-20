add_executable(weave_numerics_tests
    Int128.cxx
    UInt128.cxx
)

target_link_libraries(weave_numerics_tests PUBLIC weave_numerics)
target_link_libraries(weave_numerics_tests PUBLIC thirdparty_catch2)

WEAVE_CXX_FORTIFY_CODE(weave_numerics_tests)

add_test(
    NAME        weave_numerics_tests
    COMMAND     weave_numerics_tests
)
