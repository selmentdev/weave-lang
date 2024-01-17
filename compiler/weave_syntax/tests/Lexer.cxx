#include "weave/platform/Compiler.hxx"
#include "weave/syntax/Lexer.hxx"

WEAVE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

WEAVE_EXTERNAL_HEADERS_END

void LexTokenStream(
    std::string_view source,
    std::vector<weave::syntax::SyntaxKind>& result,
    weave::source::DiagnosticSink& diagnostic)
{
    weave::source::SourceText text{std::string{source}};

    weave::syntax::Lexer lexer{diagnostic, text, weave::syntax::LexerTriviaMode::All};

    weave::syntax::TokenInfo token{};
    while (lexer.Lex(token))
    {
        if (token.Kind == weave::syntax::SyntaxKind::EndOfFileToken)
        {
            break;
        }

        result.push_back(token.Kind);
    }
}

TEST_CASE("Merged operators")
{
    std::vector<weave::syntax::SyntaxKind> tokens{};
    weave::source::DiagnosticSink diagnostic{};

    LexTokenStream("<<<<", tokens, diagnostic);

    REQUIRE(tokens.size() == 2);
    REQUIRE(tokens[0] == weave::syntax::SyntaxKind::LessThanLessThanToken);
    REQUIRE(tokens[1] == weave::syntax::SyntaxKind::LessThanLessThanToken);
}
