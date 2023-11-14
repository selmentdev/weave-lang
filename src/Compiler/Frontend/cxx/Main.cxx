#include "Weave.Core/Platform/Windows.hxx"
#include "Weave.Core/HandleTable.hxx"
#include "Weave.Core/Diagnostic.hxx"
#include "Weave.Core/IO/FileHandle.hxx"
#include "Weave.Core/IO/FileSystem.hxx"
#include "Weave.Core/IO/FileReader.hxx"
#include "Weave.Core/IO/FileWriter.hxx"

#include <fmt/format.h>
#include <fmt/chrono.h>

#include "Weave.Core/Assert.hxx"
#include "Weave.Core/Memory/Allocator.hxx"
#include "Weave.Syntax/Token.hxx"
#include "Weave.Syntax/LexerContext.hxx"
#include "Weave.Syntax/Lexer.hxx"
#include "Weave.Core/CommandLine.hxx"

#include <charconv>
#include <chrono>
#include <cstdlib>

int main(int argc, const char* argv[])
{
    Weave::CommandLineBuilder builder{};

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


    std::span args{argv+1, static_cast<size_t>(argc-1)};

    if (auto matched = builder.Parse(args); matched.has_value())
    {
        if (matched->HasFlag("help"))
        {
            fmt::println("Usage: {} [OPTIONS]", argv[0]);
            fmt::println("");

            for (auto const& option : builder.GetOptions())
            {
                fmt::println("{}", Weave::FormatOption(option));
                fmt::println("\t{}", option.Description);
                fmt::println("");
            }
            return EXIT_SUCCESS;
        }

        auto const& result = matched.value();

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

        using namespace Weave::Syntax;
        using namespace Weave::IO;
        using namespace Weave;

        if (auto file = ReadTextFile(files.front()); file.has_value())
        {
            std::vector<Token*> tokens{};
            DiagnosticSink diagnostic{"<source>"};
            SourceText text{std::move(*file)};
            LexerContext context{};
            Lexer lexer{diagnostic, text, TriviaMode::All};

            auto started = std::chrono::high_resolution_clock::now();

            while (lexer.Lex())
            {
                for (auto const& t : lexer.GetLeadingTrivia())
                {
                    LineSpan const loc = text.GetLineSpan(t.Source);
                    fmt::println("ll: {}, {}:{}-{}:{}: {}",
                        TriviaKindTraits::GetName(t.Kind),
                        loc.Start.Line, loc.Start.Column, loc.End.Line, loc.End.Column,
                        t.Source.End.Offset - t.Source.Start.Offset);
                }
                {
                    LineSpan const loc = text.GetLineSpan(lexer.GetSpan());
                    fmt::println("kk: {} ('{}'): '{}' '{}' '{}', {}:{}-{}:{}: {}",
                        TokenKindTraits::GetName(lexer.GetKind()),
                        TokenKindTraits::GetSpelling(lexer.GetKind()),
                        lexer.GetPrefix(), lexer.GetValue(), lexer.GetSuffix(),
                        loc.Start.Line, loc.Start.Column, loc.End.Line, loc.End.Column,
                        lexer.GetSpan().End.Offset - lexer.GetSpan().Start.Offset);
                }
                for (auto const& t : lexer.GetTrailingTrivia())
                {
                    LineSpan const loc = text.GetLineSpan(t.Source);
                    fmt::println("tt: {}, {}:{}-{}:{}: {}",
                        TriviaKindTraits::GetName(t.Kind),
                        loc.Start.Line, loc.Start.Column, loc.End.Line, loc.End.Column,
                        t.Source.End.Offset - t.Source.Start.Offset);
                }

                if (lexer.GetKind() == TokenKind::EndOfFile)
                {
                    break;
                }
            }

            auto finished = std::chrono::high_resolution_clock::now();
            context.Strings.Dump();

            std::chrono::duration<double, std::milli> const lexed = finished - started;

            std::vector<std::string> diag{};
            FormatDiagnostics(diag, text, diagnostic, 1000);

            for (std::string const& item : diag)
            {
                fmt::println(stderr, "{}", item);
            }

            size_t allocated{};
            size_t reserved{};
            context.QueryMemoryUsage(allocated, reserved);
            context.DumpMemoryUsage();

            fmt::println("lexed in {}", lexed);
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
