add_executable(Weave.Syntax.Tests
 "Main.cxx" "TokenKind.cxx")

target_link_libraries(Weave.Syntax.Tests PUBLIC Weave.Syntax)
target_link_libraries(Weave.Syntax.Tests PUBLIC Catch2)
WEAVE_CXX_FORTIFY_CODE(Weave.Syntax.Tests)

add_test(
    NAME        Weave.Syntax.Tests
    COMMAND     Weave.Syntax.Tests
)
