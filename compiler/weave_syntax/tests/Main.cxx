#include "weave/platform/Compiler.hxx"
#include "weave/syntax/Parser.hxx"

#include <array>
#include <span>

WEAVE_EXTERNAL_HEADERS_BEGIN

#include <catch_amalgamated.hpp>

WEAVE_EXTERNAL_HEADERS_END

namespace helpers
{
    struct StructureMatcher
    {
    };

    struct DiagnosticMatcher
    {
        std::string_view Message{};
        weave::source::LineSpan Span{};
    };

    // IDEA:
    // 1. Parse source code into AST.
    // 2. Validate expected diagnostic messages.
    // 3. Walk AST linearly and validate expected structure.

    void Validate(
        std::string_view source,
        std::span<DiagnosticMatcher const> expectedDiagnostics)
    {
        using namespace weave;
        source::SourceText text{std::string{source}};
        source::DiagnosticSink diagnostic{"<source>"};
        syntax::SyntaxFactory factory{};

        syntax::Parser parser{&diagnostic, &factory, text};
        [[maybe_unused]] syntax::SourceFileSyntax const* cu = parser.ParseSourceFile();

        REQUIRE(diagnostic.Items.size() == expectedDiagnostics.size());

        for (size_t i = 0; i < diagnostic.Items.size(); ++i)
        {
            auto const& actual = diagnostic.Items[i];
            auto const& expected = expectedDiagnostics[i];

            auto actualSource = text.GetLineSpan(actual.Source);

            CHECK(actual.Message == expected.Message);
            CHECK(actualSource.Start.Line == expected.Span.Start.Line);
            CHECK(actualSource.Start.Column == expected.Span.Start.Column);
            CHECK(actualSource.End.Line == expected.Span.End.Line);
            CHECK(actualSource.End.Column == expected.Span.End.Column);
        }
    }
}


TEST_CASE("Empty")
{
    helpers::Validate("", {});
}
