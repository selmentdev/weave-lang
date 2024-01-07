add_executable(weave_syntax_tests
    Main.cxx
    SyntaxKind.cxx
)

target_link_libraries(weave_syntax_tests PUBLIC weave_syntax)
target_link_libraries(weave_syntax_tests PUBLIC thirdparty_catch2)
WEAVE_CXX_FORTIFY_CODE(weave_syntax_tests)

add_test(
    NAME        weave_syntax_tests
    COMMAND     weave_syntax_tests
)

add_test(
    NAME        weave-compiletest-syntax-tests
    COMMAND     weave_compiletest
        --exe $<TARGET_FILE:weave_driver>
        --working-directory ${CMAKE_CURRENT_SOURCE_DIR}
        -O ${CMAKE_CURRENT_BINARY_DIR}
        this is positional parameter
        -C name=value
        -C opt=3
        -C debug=false
        --
        --unknown-switch
        positional
        --options
        -a -r -e
        not
        --parsed "as is"
)
