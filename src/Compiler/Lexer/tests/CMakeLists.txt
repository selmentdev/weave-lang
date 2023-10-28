add_executable(Weave.Lexer.Tests
 "Main.cxx")

target_link_libraries(Weave.Lexer.Tests PUBLIC Weave.Lexer)
target_link_libraries(Weave.Lexer.Tests PUBLIC Catch2)
WEAVE_CXX_FORTIFY_CODE(Weave.Lexer.Tests)

add_test(
    NAME        Weave.Lexer.Tests
    COMMAND     Weave.Lexer.Tests
)
