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

#include <charconv>
#include <chrono>

int main()
{
    {
        double dv{};
        std::string_view sv{"21.37"};
        auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), dv, std::chars_format::general);
        WEAVE_ASSERT(ec == std::errc{});
        fmt::println("{}, {}", dv, std::to_underlying(ec));
    }
    {
        using namespace Weave::Syntax;
        using namespace Weave::IO;
        using namespace Weave;

#if defined(WIN32)
        //if (auto file = ReadTextFile(R"(D:\repos\runtime\src\tests\JIT\jit64\opt\cse\hugeexpr1.cs)"))
        // if (auto file = ReadTextFile(R"(D:\repos\rust\library\stdarch\crates\core_arch\src\aarch64\neon\generated.rs)"))
         //if (auto file = ReadTextFile(R"(D:\repos\weave-lang\src\Compiler\Syntax\tests\data\Numbers.source)"))
         if (auto file = ReadTextFile(R"(D:\repos\weave-lang\src\Compiler\Syntax\tests\data\Strings.source)"))
        //   if (auto file = ReadTextFile(R"(D:\test\windows.ui.xaml.controls.h)"))
        //if (auto file = ReadTextFile(R"(D:\test\d3d12.h)"))
#else
        if (auto file = ReadTextFile(R"(/usr/include/c++/12/vector)"))
#endif
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
    }
    return 0;
}
