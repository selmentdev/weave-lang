#include "Weave.Core/Assert.hxx"
#include "Weave.Core/Process.hxx"
#include "Weave.Core/CommandLine.hxx"
#include "Weave.Core/IO/FileSystem.hxx"
#include <span>
#include <string_view>
#include <vector>
#include <functional>
#include <variant>
#include <map>
#include <filesystem>
#include <expected>


int main(int argc, char** argv)
{
    fmt::println("--- args-begin ---");
    for (int i = 0; i < argc; ++i)
    {
        fmt::println("{}: '{}'", i, argv[i]);
    }
    fmt::println("--- args-end ---");

    using namespace Weave;
    CommandLineBuilder parser{};
    parser
        .RequiredSingle("exe", "", "Path to executable", "<file>")
        .RequiredSingle("working-directory", "W", "Working directory", "<dir>")
        .RequiredSingle("", "O", "Output", "<file>")
        .Multiple("codegen", "C", "Code generator options", "<name>[=<value>]")
        .Multiple("config", "", "Configuration", "<release|debug|checked>")
        .Flag("verbose", "v", "Use verbose output", "")
        .Flag("version", "V", "Prints version information", "")
        .Flag("help", "h", "Prints help", "");

    if (auto r = parser.Parse(std::span{const_cast<const char**>(argv + 1), static_cast<size_t>(argc - 1)}); !r.has_value())
    {
        fmt::println(stderr, "{}", r.error());
        return EXIT_FAILURE;
    }
    else
    {
        CommandLineParseResult const& m = *r;

        if (auto v = m.GetValue("exe"))
        {
            fmt::println("exe: {}", *v);
        }

        if (auto v = m.GetValue("working-directory"))
        {
            fmt::println("dir: {}", *v);
        }

        for (auto v : m.GetValues("codegen"))
        {
            fmt::println("codegen: {}", v);
        }

        for (auto v : m.GetPositional())
        {
            fmt::println("positional: {}", v);
        }

        if (m.HasFlag("help"))
        {
            for (auto const& option : parser.GetOptions())
            {
                //fmt::println("    {:>2} {} {}", option.ShortName, option.Name, option.Hint);

                fmt::println("    {}", FormatOption(option));

                if (not option.Description.empty())
                {
                    fmt::println("                {}", option.Description);
                }
            }

            return EXIT_SUCCESS;
        }

        auto exeName = m.GetValue("exe");
        auto workingDirectory = m.GetValue("working-directory");
        auto compilerArgs = m.GetValues("codegen");

        // exeName = R"(D:\repos\weave-lang\out\build\x64-Debug\src\Compiler\Frontend\weave-frontend.exe)";
        // workingDirectory = R"(D:\repos\weave-lang\src\Compiler\Syntax\tests)";
        std::filesystem::path const wd{workingDirectory.value()};

        std::filesystem::recursive_directory_iterator it{wd};

        for (auto const& entry : it)
        {
            if (entry.path().extension() != ".source")
            {
                continue;
            }

            fmt::println("{}", entry.path().string());

            std::string output{};
            std::string error{};

            if (auto ret = Weave::Execute(std::string{exeName.value()}.c_str(), entry.path().string().c_str(), nullptr, output, error))
            {
                std::filesystem::path outputFilePath = entry.path();
                outputFilePath.replace_extension(".output");

                std::filesystem::path errorFilePath = entry.path();
                errorFilePath.replace_extension(".error");

                // Try to read output and error files
                if (auto const expected = Weave::IO::ReadTextFile(outputFilePath.string()).value_or(""); expected != output)
                {
                    fmt::println("output file content does not match");

                    Weave::IO::WriteTextFile(outputFilePath.string(), output);
                }
                
                if (auto const expected = Weave::IO::ReadTextFile(errorFilePath.string()).value_or(""); expected != error)
                {
                    fmt::println("output file content does not match");

                    Weave::IO::WriteTextFile(errorFilePath.string(), error);
                }
            }
            else
            {
                fmt::println("Failed to spawn child process");
            }
        }
    }

    return EXIT_SUCCESS;
}
