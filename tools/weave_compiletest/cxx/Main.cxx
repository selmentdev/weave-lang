#include "weave/Version.hxx"
#include "weave/commandline/CommandLineParser.hxx"
#include "weave/system/Process.hxx"
#include "weave/filesystem/FileSystem.hxx"

#include <span>
#include <string_view>
#include <vector>
#include <functional>
#include <variant>
#include <map>
#include <filesystem>
#include <utility>
#include <expected>

#include <fmt/format.h>

void NormalizeLineEndings(std::string& value)
{
    // Replace CRLF with LF
    std::string::size_type n;

    while ((n = value.find("\r\n")) != std::string::npos)
    {
        value.replace(n, 2, "\n");
    }
}

int main(int argc, char** argv)
{
    fmt::println("--- args-begin ---");
    for (int i = 0; i < argc; ++i)
    {
        fmt::println("{}: '{}'", i, argv[i]);
    }
    fmt::println("--- args-end ---");

    weave::commandline::ArgumentEnumerator enumerator{argc, argv};

    weave::commandline::ArgumentParser parser{};
    parser.AddOption("-exe", "Path to executable", "path");
    parser.AddOption("-working-directory", "Working directory", "path");
    parser.AddOption("-o", "Output", "path");
    parser.AddOption("-c", "Code generator options", "name=value");
    parser.AddOption("-config", "Configuration", "<release|debug|checked>");
    parser.AddOption("-verbose", "Use verbose output");
    parser.AddOption("-version", "Prints version information");
    parser.AddOption("-help", "Prints help");

    if (auto r = parser.Parse(enumerator); not r.has_value())
    {
        fmt::println(stderr, "{}", r.error().Argument);
        return EXIT_FAILURE;
    }
    else
    {
        if (auto v = r->GetValue("-exe"))
        {
            fmt::println("exe: {}", *v);
        }

        if (auto v = r->GetValue("-wworking-directory"))
        {
            fmt::println("dir: {}", *v);
        }

        for (auto v : r->GetValues("-c"))
        {
            fmt::println("codegen: {}", v);
        }

        for (auto v : r->GetPositional())
        {
            fmt::println("positional: {}", v);
        }

        if (r->Contains("-help"))
        {
            parser.PrintUsage("weave-compiletest");

            return EXIT_SUCCESS;
        }

        if (r->Contains("-version"))
        {
            fmt::println(stdout, "weave-compiletest version {}", WEAVE_LANG_VERSION);
            return EXIT_SUCCESS;
        }

        auto exeName = r->GetValue("-exe");
        auto workingDirectory = r->GetValue("-working-directory");
        auto compilerArgs = r->GetValues("-c");

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

            std::string executable{exeName.value()};
            std::string args{};
            args += fmt::format("\"{}\"", entry.path().string());
            args += " -x:print-syntax-tree";

            if (auto ret = weave::system::Execute(executable.c_str(), args.c_str(), wd.string().c_str(), output, error))
            {
                NormalizeLineEndings(output);
                NormalizeLineEndings(error);

                std::filesystem::path outputFilePath = entry.path();
                outputFilePath.replace_extension(".output");

                std::filesystem::path errorFilePath = entry.path();
                errorFilePath.replace_extension(".error");

                bool shouldWriteOutputFile = false;
                bool shouldWriteErrorFile = false;
                // Try to read output and error files
                if (auto const expected = weave::filesystem::ReadTextFile(outputFilePath.string()))
                {
                    if (*expected != output)
                    {
                        fmt::println("output file content does not match");
                        shouldWriteOutputFile = true;
                    }
                }
                else
                {
                    fmt::println(stderr, "Failed to open file '{}' ('{}')", outputFilePath.string(), std::to_underlying(expected.error()));
                    shouldWriteOutputFile = true;
                }

                if (shouldWriteOutputFile)
                {
                    weave::filesystem::WriteTextFile(outputFilePath.string(), output);
                }

                if (auto const expected = weave::filesystem::ReadTextFile(errorFilePath.string()))
                {
                    if (*expected != error)
                    {
                        fmt::println("output file content does not match");
                        shouldWriteErrorFile = true;
                    }
                }
                else
                {
                    fmt::println(stderr, "Failed to open file '{}' ('{}')", errorFilePath.string(), std::to_underlying(expected.error()));
                    shouldWriteErrorFile = true;
                }

                if (shouldWriteErrorFile)
                {
                    weave::filesystem::WriteTextFile(errorFilePath.string(), error);
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
