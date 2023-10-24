add_executable(Weave.Lexer.Tests
 "Main.cxx")

target_link_libraries(Weave.Lexer.Tests PUBLIC Weave.Lexer)
target_link_libraries(Weave.Lexer.Tests PUBLIC Catch2)

add_test(
    NAME        Weave.Lexer.Tests
    COMMAND     Weave.Lexer.Tests
)
