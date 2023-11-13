add_executable(Weave.Syntax.Tests
 "Main.cxx" "TokenKind.cxx")

target_link_libraries(Weave.Syntax.Tests PUBLIC Weave.Syntax)
target_link_libraries(Weave.Syntax.Tests PUBLIC Catch2)
WEAVE_CXX_FORTIFY_CODE(Weave.Syntax.Tests)

add_test(
    NAME        Weave.Syntax.Tests
    COMMAND     Weave.Syntax.Tests
)

add_test(
    NAME        weave-compiletest-syntax-tests
    COMMAND     Weave.CompileTest
        --exe $<TARGET_FILE:Weave.Frontend>
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
