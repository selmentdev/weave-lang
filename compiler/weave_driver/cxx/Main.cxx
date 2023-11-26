#include <fmt/format.h>
#include <fmt/chrono.h>


#include <charconv>
#include <cstdlib>

#include "weave/Session.hxx"
#include "weave/core/String.hxx"
#include "weave/CommandLine.hxx"
#include "weave/tokenizer/Token.hxx"
#include "weave/filesystem/FileSystem.hxx"
#include "weave/source/Diagnostic.hxx"
#include "weave/tokenizer/Tokenizer.hxx"
#include "weave/tokenizer/TokenizerContext.hxx"
#include "weave/time/Instant.hxx"
#include "weave/Session.hxx"

int main(int argc, const char* argv[])
{
    using namespace weave;

    commandline::CommandLineBuilder builder{};

    builder.Multiple(
        "codegen",
        "c",
        "Code generator options",
        "<name=value>");

    builder.Multiple(
        "emit",
        "e",
        "List of types of output to emit",
        "items");

    builder.Multiple(
        "experimental",
        "x",
        "Experimental options",
        "name[=value]");

    builder.Flag(
        "help",
        "h",
        "Prints help");

    builder.Flag(
        "verbose",
        "v",
        "Use verbose output");

    if (auto matched = builder.Parse(argc, argv); matched.has_value())
    {
        if (matched->HasFlag("help"))
        {
            fmt::println("Usage: {} [OPTIONS]", argv[0]);
            fmt::println("");

            for (auto const& option : builder.GetOptions())
            {
                fmt::println("{}", commandline::FormatOption(option));
                fmt::println("\t{}", option.Description);
                fmt::println("");
            }
            return EXIT_SUCCESS;
        }

        weave::errors::Handler handler{};
        auto cmd = weave::session::CodeGeneratorOptions::FromCommandLine(handler, *matched);

        for (auto const& message : handler.GetMessages())
        {
            fmt::println(stderr, "codegen errors: {}", message.Value);
        }

        fmt::println("cmd.debug: {}", cmd.Debug);


        auto const& result = matched.value();

        session::CodeGeneratorOptions codegen = session::CodeGeneratorOptions::FromCommandLine(handler, *matched);
        codegen.DebugPrint();

        auto const& files = result.GetPositional();

        fmt::println("---");

        for (auto const& f : files)
        {
            fmt::println("file: {}", f);
        }

        fmt::println("___");

        if (files.empty())
        {
            fmt::println(stderr, "No input files specified");
            return EXIT_FAILURE;
        }

        if (files.size() > 1)
        {
            fmt::println(stderr, "Too many files specified");
            return EXIT_FAILURE;
        }


        if (auto file = filesystem::ReadTextFile(files.front()); file.has_value())
        {
            std::vector<tokenizer::Token*> tokens{};
            source::DiagnosticSink diagnostic{"<source>"};
            source::SourceText text{std::move(*file)};
            tokenizer::TokenizerContext context{};
            tokenizer::Tokenizer lexer{diagnostic, text, tokenizer::TriviaMode::All};

            time::Instant const started = time::Instant::Now();

            while (lexer.Lex())
            {
                for (auto const& t : lexer.GetLeadingTrivia())
                {
                    source::LineSpan const loc = text.GetLineSpan(t.Source);
                    fmt::println("ll: {}, {}:{}-{}:{}: {}",
                        tokenizer::TriviaKindTraits::GetName(t.Kind),
                        loc.Start.Line, loc.Start.Column, loc.End.Line, loc.End.Column,
                        t.Source.End.Offset - t.Source.Start.Offset);
                }
                {
                    source::LineSpan const loc = text.GetLineSpan(lexer.GetSpan());
                    fmt::println("kk: {} ('{}'): '{}' '{}' '{}', {}:{}-{}:{}: {}",
                        tokenizer::TokenKindTraits::GetName(lexer.GetKind()),
                        tokenizer::TokenKindTraits::GetSpelling(lexer.GetKind()),
                        lexer.GetPrefix(), lexer.GetValue(), lexer.GetSuffix(),
                        loc.Start.Line, loc.Start.Column, loc.End.Line, loc.End.Column,
                        lexer.GetSpan().End.Offset - lexer.GetSpan().Start.Offset);
                }
                for (auto const& t : lexer.GetTrailingTrivia())
                {
                    source::LineSpan const loc = text.GetLineSpan(t.Source);
                    fmt::println("tt: {}, {}:{}-{}:{}: {}",
                        tokenizer::TriviaKindTraits::GetName(t.Kind),
                        loc.Start.Line, loc.Start.Column, loc.End.Line, loc.End.Column,
                        t.Source.End.Offset - t.Source.Start.Offset);
                }

                if (lexer.GetKind() == tokenizer::TokenKind::EndOfFile)
                {
                    break;
                }
            }

            time::Duration const timeout = started.QueryElapsed();
            context.Strings.Dump();

            std::vector<std::string> diag{};
            source::FormatDiagnostics(diag, text, diagnostic, 1000);

            for (std::string const& item : diag)
            {
                fmt::println(stderr, "{}", item);
            }

            size_t allocated{};
            size_t reserved{};
            context.QueryMemoryUsage(allocated, reserved);
            context.DumpMemoryUsage();

            fmt::println("lexed in {} ms", timeout.AsMilliseconds());
            fmt::println("allocated: {}, reserved: {}", allocated, reserved);
        }
        else
        {
            fmt::println(stderr, "Failed to open file: {}", files.front());
        }
    }
    else
    {
        fmt::println(stderr, "{}", matched.error());
        return EXIT_FAILURE;
    }

    return 0;
}
