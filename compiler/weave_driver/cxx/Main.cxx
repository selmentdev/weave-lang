#include <fmt/format.h>
#include <fmt/chrono.h>


#include <charconv>
#include <cstdlib>

#include "weave/Session.hxx"
#include "weave/core/String.hxx"
#include "weave/CommandLine.hxx"
#include "weave/syntax/Token.hxx"
#include "weave/filesystem/FileSystem.hxx"
#include "weave/source/Diagnostic.hxx"
#include "weave/syntax/Lexer.hxx"
#include "weave/syntax/LexerContext.hxx"
#include "weave/time/Instant.hxx"

namespace weave
{
    struct SanitizersSpec final
    {
        bool AddressSanitizer{};
        bool ThreadSanitizer{};
        bool MemorySanitizer{};
        bool LeakSanitizer{};

        void DebugPrint()
        {
            fmt::println("address-sanitizer: {}", this->AddressSanitizer);
            fmt::println("thread-sanitizer: {}", this->ThreadSanitizer);
            fmt::println("memory-sanitizer: {}", this->MemorySanitizer);
            fmt::println("leak-sanitizer: {}", this->LeakSanitizer);
        }
    };

    std::optional<SanitizersSpec> ParseSanitizersSpec(std::string_view value)
    {
        SanitizersSpec result{};

        for (std::string const& item : core::Split(value, ','))
        {
            if (item == "address")
            {
                result.AddressSanitizer = true;
            }
            else if (item == "thread")
            {
                result.ThreadSanitizer = true;
            }
            else if (item == "memory")
            {
                result.MemorySanitizer = true;
            }
            else if (item == "leak")
            {
                result.MemorySanitizer = true;
            }
            else
            {
                return std::nullopt;
            }
        }

        return result;
    }

    class CodeGeneratorOptions
    {
    public:
        std::vector<std::string> Defines{};
        int OptimizationLevel = 0;
        bool Debug = false;
        std::string Target{};
        std::string Platform{};
        bool CheckOverflow = false;
        SanitizersSpec Sanitizers{};

    private:
        static std::optional<std::string_view> Split(std::string_view item, std::string_view& name)
        {
            auto const separator = item.find('=');

            if (separator != std::string_view::npos)
            {
                name = item.substr(0, separator);
                return item.substr(separator + 1);
            }
            else
            {
                name = item;
                return std::nullopt;
            }
        }

        static std::optional<int> ParseInt(std::string_view value)
        {
            int result{};
            auto const& [p, ec] = std::from_chars(value.data(), value.data() + value.size(), result);

            if (ec != std::errc())
            {
                return std::nullopt;
            }

            return result;
        }

        static std::optional<bool> ParseBool(std::string_view value)
        {
            if (value == "true")
            {
                return true;
            }

            if (value == "false")
            {
                return false;
            }

            return std::nullopt;
        }

    public:
        bool Parse(commandline::CommandLineParseResult const& result)
        {
            auto const& options = result.GetValues("codegen");

            for (auto const& option : options)
            {
                std::string_view name{};
                std::optional<std::string_view> value = Split(option, name);

                if (value.has_value())
                {
                    if (name == "define")
                    {
                        this->Defines.emplace_back(*value);
                    }
                    else if (name == "optimization-level")
                    {
                        if (auto parsed = ParseInt(*value))
                        {
                            this->OptimizationLevel = *parsed;
                        }
                        else
                        {
                            fmt::println(stderr, "Invalid optimization level: {}", *value);
                            return false;
                        }
                    }
                    else if (name == "debug")
                    {
                        if (auto parsed = ParseBool(*value))
                        {
                            this->Debug = *parsed;
                        }
                        else
                        {
                            fmt::println(stderr, "Invalid debug flag: {}", *value);
                            return false;
                        }
                    }
                    else if (name == "target")
                    {
                        this->Target = value.value();
                    }
                    else if (name == "platform")
                    {
                        this->Platform = value.value();
                    }
                    else if (name == "sanitizers")
                    {
                        if (auto parsed = ParseSanitizersSpec(*value))
                        {
                            this->Sanitizers = *parsed;
                        }
                        else
                        {
                            fmt::println(stderr, "Invalid sanitizers: {}", *value);
                            return false;
                        }
                    }
                    else if (name == "check-overflow")
                    {
                        if (auto parsed = ParseBool(*value))
                        {
                            this->CheckOverflow = *parsed;
                        }
                        else
                        {
                            fmt::println(stderr, "Invalid check overflow flag: {}", *value);
                            return false;
                        }
                    }
                    else
                    {
                        fmt::println(stderr, "Unknown codegen option: {}", name);
                        return false;
                    }
                }
                else
                {
                    fmt::println(stderr, "Value expected for codegen option: {}", name);
                }
            }

            return true;
        }

        void DebugPrint()
        {
            fmt::println("defines: {}", this->Defines.size());

            for (auto const& define : this->Defines)
            {
                fmt::println("define: {}", define);
            }

            fmt::println("optimization level: {}", this->OptimizationLevel);
            fmt::println("check overflow: {}", this->CheckOverflow);
            fmt::println("debug: {}", this->Debug);
            fmt::println("target: {}", this->Target);
            fmt::println("platform: {}", this->Platform);

            this->Sanitizers.DebugPrint();
        }
    };
}

namespace weave::endian
{
    constexpr uint16_t LoadUnaligned16(std::byte const* buffer)
    {
        if (std::is_constant_evaluated())
        {
            return static_cast<uint16_t>(
                (static_cast<uint8_t>(buffer[0]) << 0) |
                (static_cast<uint8_t>(buffer[1]) << 8));
        }
        else
        {
            uint16_t result;
            memcpy(&result, buffer, sizeof(result));
            return result;
        }
    }

    constexpr uint32_t LoadUnaligned32(std::byte const* buffer)
    {
        if (std::is_constant_evaluated())
        {
            return static_cast<uint32_t>(
                (static_cast<uint8_t>(buffer[0]) << 0) |
                (static_cast<uint8_t>(buffer[1]) << 8) |
                (static_cast<uint8_t>(buffer[2]) << 16) |
                (static_cast<uint8_t>(buffer[3]) << 24));
        }
        else
        {
            uint32_t result;
            memcpy(&result, buffer, sizeof(result));
            return result;
        }
    }

    template <std::endian E, typename T>
    auto Load(std::byte const* source) -> T
        requires(std::is_integral_v<T>)
    {
        std::byte buffer[sizeof(T)];
        std::memcpy(buffer, source, sizeof(T));

        if constexpr (E != std::endian::native)
        {
            return std::byteswap(std::bit_cast<T>(buffer));
        }
        else
        {
            return std::bit_cast<T>(buffer);
        }
    }

    template <std::endian E, typename T>
    auto Store(std::byte* destination, T const& source)
        requires(std::is_integral_v<T>)
    {
        if constexpr (E != std::endian::native)
        {
            T const swapped = std::byteswap(source);
            std::memcpy(destination, &swapped, sizeof(T));
        }
        else
        {
            std::memcpy(destination, &source, sizeof(T));
        }
    }

}

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

        CodeGeneratorOptions codegen{};
        codegen.Parse(result);

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
            std::vector<syntax::Token*> tokens{};
            source::DiagnosticSink diagnostic{"<source>"};
            source::SourceText text{std::move(*file)};
            syntax::LexerContext context{};
            syntax::Lexer lexer{diagnostic, text, syntax::TriviaMode::All};

            time::Instant const started = time::Instant::Now();

            while (lexer.Lex())
            {
                for (auto const& t : lexer.GetLeadingTrivia())
                {
                    source::LineSpan const loc = text.GetLineSpan(t.Source);
                    fmt::println("ll: {}, {}:{}-{}:{}: {}",
                        syntax::TriviaKindTraits::GetName(t.Kind),
                        loc.Start.Line, loc.Start.Column, loc.End.Line, loc.End.Column,
                        t.Source.End.Offset - t.Source.Start.Offset);
                }
                {
                    source::LineSpan const loc = text.GetLineSpan(lexer.GetSpan());
                    fmt::println("kk: {} ('{}'): '{}' '{}' '{}', {}:{}-{}:{}: {}",
                        syntax::TokenKindTraits::GetName(lexer.GetKind()),
                        syntax::TokenKindTraits::GetSpelling(lexer.GetKind()),
                        lexer.GetPrefix(), lexer.GetValue(), lexer.GetSuffix(),
                        loc.Start.Line, loc.Start.Column, loc.End.Line, loc.End.Column,
                        lexer.GetSpan().End.Offset - lexer.GetSpan().Start.Offset);
                }
                for (auto const& t : lexer.GetTrailingTrivia())
                {
                    source::LineSpan const loc = text.GetLineSpan(t.Source);
                    fmt::println("tt: {}, {}:{}-{}:{}: {}",
                        syntax::TriviaKindTraits::GetName(t.Kind),
                        loc.Start.Line, loc.Start.Column, loc.End.Line, loc.End.Column,
                        t.Source.End.Offset - t.Source.Start.Offset);
                }

                if (lexer.GetKind() == syntax::TokenKind::EndOfFile)
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
