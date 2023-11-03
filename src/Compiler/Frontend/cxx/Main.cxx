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


#include <chrono>

int main()
{
    {
        using namespace Weave::Syntax;
        using namespace Weave::IO;
        using namespace Weave;

        // if (auto file = ReadTextFile(R"(D:\repos\runtime\src\tests\JIT\jit64\opt\cse\hugeexpr1.cs)"))
        if (auto file = ReadTextFile(R"(D:\repos\rust\library\stdarch\crates\core_arch\src\aarch64\neon\generated.rs)"))
        // if (auto file = ReadTextFile(R"(D:\repos\weave-lang\src\Compiler\Syntax\tests\data\Numbers.source)"))
        //  if (auto file = ReadTextFile(R"(D:\test\windows.ui.xaml.controls.h)"))
        //  if (auto file = ReadTextFile(R"(D:\test\d3d12.h)"))
        {
            std::vector<Token*> tokens{};
            DiagnosticSink diagnostic{"<source>"};
            SourceText text{std::move(*file)};
            LexerContext context{};
            Lexer lexer{diagnostic, context, text, TriviaMode::All};

            auto started = std::chrono::high_resolution_clock::now();

            while (true)
            {
                Token* token = lexer.Lex();

                if (token == nullptr)
                {
                    break;
                }

                tokens.push_back(token);

                if (token->Kind == TokenKind::EndOfFile)
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
                fmt::println("{}", item);
            }

            size_t allocated{};
            size_t reserved{};
            context.QueryMemoryUsage(allocated, reserved);

            fmt::println("lexed in {}", lexed);
            fmt::println("allocated: {}, reserved: {}", allocated, reserved);
        }
    }
    return 0;
}
