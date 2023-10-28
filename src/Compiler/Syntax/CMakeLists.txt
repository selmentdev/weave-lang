add_library(Weave.Syntax STATIC
 "cxx/Token.cxx"
 "cxx/TokenKind.cxx"
 "cxx/TriviaKind.cxx"
 "cxx/CharTraits.cxx" "cxx/Lexer.cxx")

target_include_directories(Weave.Syntax PUBLIC include)
target_link_libraries(Weave.Syntax PUBLIC Weave.Core)
WEAVE_CXX_FORTIFY_CODE(Weave.Syntax)

add_subdirectory(tests)
