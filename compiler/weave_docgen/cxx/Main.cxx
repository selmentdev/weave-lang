#include "weave/Version.hxx"
#include "weave/CommandLine.hxx"
#include "weave/filesystem/DirectoryEnumerator.hxx"

#include <utility>

struct Options
{
    std::string SourcePath{};

    static std::expected<Options, weave::commandline::CommandLineErrorResult> Parse(int argc, const char* argv[])
    {
        Options options;
        weave::commandline::CommandLineBuilder builder{};
        builder.RequiredSingle("source", "s", "source path", "SOURCE");
        builder.Flag("verbose", "v", "Enable verbose mode");
        builder.Flag("help", "h", "Print this help message");

        auto result = builder.Parse(argc, argv);
        if (!result)
        {
            return std::unexpected(result.error());
        }

        options.SourcePath = result->GetValue("source").value_or("");

        return options;
    }
};

int main(int argc, const char* argv[])
{
    fmt::println("[branch: {}, hash: {}]",
        WEAVE_PROJECT_BRANCH,
        WEAVE_PROJECT_HASH);

    fmt::println("weave-docgen");

    if (auto result = Options::Parse(argc, argv))
    {
        auto items = weave::filesystem::EnumerateDirectoryRecursive(result->SourcePath, "*.source");

        if (items)
        {
            for (auto const& item : *items)
            {
                fmt::println("{}", item);
            }
        }
        else
        {
            fmt::print("Error: {}\n", std::to_underlying(items.error()));
        }

        // weave::filesystem::DirectoryEnumerator enumerator{result->SourcePath};
        // while (auto path = enumerator.Next())
        //{
        //     (*path)->Type
        //     std::string fullpath{enumerator.Root()};
        //     weave::filesystem::path::Push(fullpath, (*path)->Path);
        //     fmt::println("{}", fullpath);
        // }
    }
    else
    {
        fmt::print("Error: {}\n", result.error().Error);
    }

    return 0;
}
