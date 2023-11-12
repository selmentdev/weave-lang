#include "Weave.Core/Assert.hxx"
#include "Weave.Core/Process.hxx"
#include <span>
#include <string_view>
#include <vector>
#include <functional>
#include <variant>
#include <map>
#include <filesystem>
// CLI schema:
// path/to/file.exe ${action} ${--command [args...]}... -- positional...

namespace Weave
{
    struct CommandLineOption
    {
        std::string_view Name;
        std::string_view ShortName;
        std::string_view Description;
        bool Required;
        bool NeedsValue;
        bool MultipleValues;
        std::function<void(std::optional<std::string_view> const& value)> Callback;
    };

    std::optional<std::string_view> Parse(
        std::span<char*> args,
        std::span<CommandLineOption const> options,
        std::vector<std::string_view>* tail)
    {
        std::map<CommandLineOption const*, size_t> counts{};
        std::optional<std::string_view> action{};

        auto arg = std::next(args.begin());

        if (arg != args.end())
        {
            if (std::string_view view_action{*arg}; !view_action.starts_with('-'))
            {
                // Consume action
                action = view_action;
                ++arg;
            }
        }

        bool positional = false;

        for (; arg != args.end(); ++arg)
        {
            std::string_view name{*arg};

            if (name == "--")
            {
                positional = true;
                continue;
            }

            if (!name.starts_with('-'))
            {
                positional = true;
            }

            if (positional)
            {
                if (tail != nullptr)
                {
                    tail->emplace_back(name);
                }
            }
            else
            {
                auto option = std::find_if(options.begin(), options.end(), [&](CommandLineOption const& opt)
                    {
                        return opt.Name == name || opt.ShortName == name;
                    });

                if (option != options.end())
                {
                    // Found option.
                    size_t& count = counts[&*option];
                    ++count;

                    if ((not option->MultipleValues) and (count != 1))
                    {
                        // Error: Option can only be specified once.
                        fmt::println("Option '{}' specified more than once", name);
                    }

                    if (option->NeedsValue)
                    {
                        // Option needs a value.
                        if (std::next(arg) < args.end())
                        {
                            // Consume value.
                            arg = std::next(arg);
                            std::string_view value{*arg};
                            option->Callback(value);
                        }
                        else
                        {
                            // Error: Missing value.
                            fmt::println("Missing value for option '{}'", name);
                        }
                    }
                    else
                    {
                        // Option does not need a value.
                        option->Callback(std::nullopt);
                    }
                }
                else
                {
                    // Error: Unknown option.
                    fmt::println("Unknown option '{}'", name);
                }
            }
        }

        // Validate required options.
        for (CommandLineOption const& option : options)
        {
            if (option.Required)
            {
                auto it = counts.find(&option);
                if (it == counts.end())
                {
                    // Error: Missing required option.
                    fmt::println("Missing required option '{}'", option.Name);
                }
            }
        }

        return action;
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

    std::optional<std::string_view> exeName{};
    std::optional<std::string_view> config{};
    std::optional<std::string_view> workingDirectory{};
    std::vector<std::string_view> compilerArgs{};
    std::vector<std::string_view> tail{};

    Weave::CommandLineOption options[]{
        {
            .Name = "--exe",
            .Required = true,
            .NeedsValue = true,
            .Callback = [&](std::optional<std::string_view> const& value)
            {
                exeName = value;
            },
        },
        {
            .Name = "--config",
            .Required = false,
            .NeedsValue = true,
            .Callback = [&](std::optional<std::string_view> const& value)
            {
                config = value;
            },
        },
        {
            .Name = "--working-directory",
            .Required = true,
            .NeedsValue = true,
            .Callback = [&](std::optional<std::string_view> const& value)
            {
                workingDirectory = value;
            },
        },
        {
            .Name = "--compiler",
            .ShortName = "-C",
            .Required = false,
            .NeedsValue = true,
            .MultipleValues = true,
            .Callback = [&](std::optional<std::string_view> const& value)
            {
                compilerArgs.emplace_back(*value);
            },
        },
    };

    auto const action = Weave::Parse(
        std::span(argv, argc),
        options,
        &tail);

    if (action == "help")
    {
        //auto max = std::max_element(std::begin(options), std::end(options), [](Weave::CommandLineOption const& left, Weave::CommandLineOption const& right)
        //    {
        //        size_t const max_left = std::max(left.Name.length(), left.ShortName.length());
        //        size_t const max_right = std::max(right.Name.length(), right.ShortName.length());
        //        return max_left < max_right;
        //    });

        fmt::println("Usage: {} action [options]", std::filesystem::path{argv[0]}.filename().string());

        for (auto const& option: options)
        {
            fmt::print("  ");

            if (not option.Name.empty())
            {
                fmt::print("{} ", option.Name);
            }

            if (not option.ShortName.empty())
            {
                fmt::print("{} ", option.ShortName);
            }

            if (not option.Description.empty())
            {
                fmt::print("- {}", option.Description);
            }

            fmt::println("");
        }

        return EXIT_SUCCESS;
    }

    fmt::println("action: {}", action.value_or("<none>"));

    fmt::println("--exe: '{}'", exeName.value_or("<nullopt>"));
    fmt::println("--working-directory: '{}'", workingDirectory.value_or("<nullopt>"));
    fmt::println("--config: '{}'", config.value_or("<nullopt>"));
    fmt::println("--- compiler-options-begin ---");
    for (auto const& item : compilerArgs)
    {
        fmt::println("-: '{}'", item);
    }
    fmt::println("--- compiler-options-end ---");
    fmt::println("--- tail-begin ---");
    for (auto const& item : tail)
    {
        fmt::println("-: '{}'", item);
    }
    fmt::println("--- tail-end ---");

    //exeName = R"(D:\repos\weave-lang\out\build\x64-Debug\src\Compiler\Frontend\weave-frontend.exe)";
    //workingDirectory = R"(D:\repos\weave-lang\src\Compiler\Syntax\tests)";
    std::filesystem::path const wd{workingDirectory.value()};

    std::filesystem::recursive_directory_iterator it{wd};

    for (auto const& entry : it)
    {
        fmt::println("{}", entry.path().string());

        if (entry.path().extension() != ".source")
        {
            continue;
        }

        std::string output{};
        std::string error{};
        
        if (auto ret = Weave::Execute(std::string{exeName.value()}.c_str(), entry.path().string().c_str(), nullptr, output, error))
        {
            fmt::println("ret: {}", *ret);

            //fmt::println("output:");
            //puts(output.c_str());

            //fmt::println("error:");
            //puts(error.c_str());
        }
        else
        {
            fmt::println("Failed to spawn child process");
        }
    }

    return EXIT_SUCCESS;
}
