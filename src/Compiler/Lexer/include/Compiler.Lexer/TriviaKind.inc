#ifndef WEAVE_TRIVIA
#define WEAVE_TRIVIA(name, spelling)
#endif

// clang-format off

WEAVE_TRIVIA(Error,                     "[trivia::error]")
WEAVE_TRIVIA(Whitespace,                "[trivia::whitespace]")
WEAVE_TRIVIA(EndOfLine,                 "[trivia::end-of-line]")
WEAVE_TRIVIA(SingleLineComment,         "[trivia::single-line-comment]")
WEAVE_TRIVIA(MultiLineComment,          "[trivia::multi-line-comment]")
WEAVE_TRIVIA(SingleLineDocumentation,   "[trivia::single-line-documentation]")
WEAVE_TRIVIA(MultiLineDocumentation,    "[trivia::multi-line-documentation]")

// clang-format on

#undef WEAVE_TRIVIA
