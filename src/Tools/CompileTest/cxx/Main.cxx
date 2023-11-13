#include "Weave.Core/Assert.hxx"
#include "Weave.Core/Process.hxx"
#include "Weave.Core/CommandLine.hxx"
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
        CommandLineErrorResult const& e = r.error();
        fmt::println(stderr, "{}: {}", e.Error, e.Option);
        return EXIT_FAILURE;
    }
    else
    {
        CommandLineParseResult const& m = *r;

        if (auto v = m.GetValue("exe"))
        {
            fmt::println("exe: {}", *v);
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
                fmt::println("ret: {}", *ret);

                // fmt::println("output:");
                // puts(output.c_str());

                // fmt::println("error:");
                // puts(error.c_str());
            }
            else
            {
                fmt::println("Failed to spawn child process");
            }
        }
    }

    return EXIT_SUCCESS;
}
