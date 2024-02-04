#include "weave/Version.hxx"
#include "weave/CommandLine.hxx"
#include "weave/filesystem/DirectoryEnumerator.hxx"

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

    return 0;
}
