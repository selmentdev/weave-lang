add_library(Weave.Lexer STATIC
 "cxx/CharTraits.cxx"
 "cxx/TokenKind.cxx"
 "cxx/TriviaKind.cxx" "cxx/TokenBuffer.cxx")

target_include_directories(Weave.Lexer PUBLIC include)
target_link_libraries(Weave.Lexer PUBLIC Weave.Core)
WEAVY_CXX_FORTIFY_CODE(Weave.Lexer)

add_subdirectory(tests)
