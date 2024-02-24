#include "weave/Version.hxx"
#include "weave/CommandLine.hxx"
#include "weave/filesystem/DirectoryEnumerator.hxx"

#include <utility>

namespace cmdline
{
    struct CommandLineOption;
    struct CommandLineGroup;

    struct CommandLineOption
    {
        std::string_view Name{};
        std::string_view Description{};
        std::string_view ValueName{};
        std::string_view DefaultValue{};
    };

    struct CommandLineGroup final
    {
        std::string_view Name{};
        std::vector<CommandLineOption> Options{};
    };

    struct CommandLineParser final
    {
        std::vector<std::string_view> _names{};
        std::vector<std::pair<std::size_t, std::string_view>> _values{};
        std::vector<std::string_view> _positional{};

        std::vector<CommandLineGroup> _groups{};

        bool Has(std::string_view name) const
        {
            for (auto const& current : this->_names)
            {
                if (current == name)
                {
                    return true;
                }
            }

            return false;
        }

        std::optional<std ::string_view> GetValue(std::string_view name) const
        {
            for (auto const& [index, value] : this->_values)
            {
                if (this->_names[index] == name)
                {
                    return value;
                }
            }

            return std::nullopt;
        }

        std::vector<std::string_view> GetValues(std::string_view name) const
        {
            std::vector<std::string_view> values{};

            for (auto const& [index, value] : this->_values)
            {
                if (this->_names[index] == name)
                {
                    values.emplace_back(value);
                }
            }

            return values;
        }

        std::span<std::string_view const> GetPositional() const
        {
            return this->_positional;
        }

        std::string _error{};

        static bool TryParseOption(
            std::string_view argument,
            std::string_view& name,
            std::optional<std::string_view>& value)
        {
            if (argument.starts_with('-'))
            {
                argument.remove_prefix(1);

                if (const auto delimiter = argument.find('='); delimiter != std::string_view::npos)
                {
                    name = argument.substr(0, delimiter);
                    value = argument.substr(delimiter + 1);
                }
                else if (argument.ends_with('+'))
                {
                    name = argument.substr(0, argument.size() - 1);
                    value = "1";
                }
                else if (argument.ends_with('-'))
                {
                    name = argument.substr(0, argument.size() - 1);
                    value = "0";
                }
                else
                {
                    name = argument;
                    value = std::nullopt;
                }

                return true;
            }

            return false;
        }

        CommandLineOption const* FindOption(std::string_view name) const
        {
            for (auto const& group : this->_groups)
            {
                for (auto const& option : group.Options)
                {
                    if (option.Name == name)
                    {
                        return &option;
                    }
                }
            }

            return nullptr;
        }

        bool Parse(int argc, char* argv[])
        {
            bool parseOptions = true;

            for (int i = 1; i < argc; ++i)
            {
                std::string_view const argument{argv[i]};

                if (parseOptions and argument.starts_with('-'))
                {
                    if (argument != "--")
                    {
                        std::string_view name{};
                        std::optional<std::string_view> value{};

                        if (TryParseOption(argument, name, value))
                        {
                            CommandLineOption const* option = this->FindOption(name);

                            if (option != nullptr)
                            {
                                this->_names.emplace_back(name);

                                if (value)
                                {
                                    this->_values.emplace_back(this->_names.size() - 1, *value);
                                }
                                else if (not option->DefaultValue.empty())
                                {
                                    this->_values.emplace_back(this->_names.size() - 1, option->DefaultValue);
                                }

                                continue;
                            }

                            this->_error = fmt::format("Invalid option: -{}", name);
                            return false;
                        }

                        this->_error = fmt::format("Invalid option: -{}", argument);
                        return false;
                    }

                    parseOptions = false;
                }
                else
                {
                    this->_positional.emplace_back(argument);
                }
            }

            return true;
        }

        void PrintHelp()
        {
            fmt::print("Usage: weave-docgen [options] [source]\n");
            fmt::print("\n");

            for (auto const& group : this->_groups)
            {
                fmt::print("{}:\n", group.Name);

                size_t maxNameLength = 0;

                for (auto const& option : group.Options)
                {
                    maxNameLength = std::max(maxNameLength, option.Name.size());
                }

                for (auto const& option : group.Options)
                {
                    fmt::print("  -{:{}}", option.Name, maxNameLength);

                    if (not option.ValueName.empty())
                    {
                        fmt::print("=<{}>", option.ValueName);
                    }

                    if (not option.Description.empty())
                    {
                        fmt::print(" {}", option.Description);
                    }

                    if (not option.DefaultValue.empty())
                    {
                        fmt::print(" (default: {})", option.DefaultValue);
                    }

                    fmt::print("\n");
                }

                fmt::print("\n");
            }
        }
    };
}

int main(int argc, char* argv[])
{
    cmdline::CommandLineParser parser{
        ._groups = {
            cmdline::CommandLineGroup{
                .Name = "Miscellaneous",
                .Options = {
                    cmdline::CommandLineOption{
                        .Name = "help",
                        .Description = "Display this help message",
                    },
                    cmdline::CommandLineOption{
                        .Name = "version",
                        .Description = "Display version information",
                    },
                    cmdline::CommandLineOption{
                        .Name = "verbose",
                        .Description = "Enable verbose output",
                        .DefaultValue = "true",
                    },
                },
            },
            cmdline::CommandLineGroup{
                .Name = "Output",
                .Options = {
                    cmdline::CommandLineOption{
                        .Name = "o:output",
                        .Description = "The output directory",
                        .ValueName = "path",
                    },
                    cmdline::CommandLineOption{
                        .Name = "o:immediate",
                        .Description = "The directory for immediate files",
                        .ValueName = "path",
                    },
                },
            },
            cmdline::CommandLineGroup{
                .Name = "Experimental",
                .Options = {
                    cmdline::CommandLineOption{
                        .Name = "x:print-syntax-tree",
                        .Description = "Prints syntax tree",
                    },
                    cmdline::CommandLineOption{
                        .Name = "x:print-semantic-tree",
                        .Description = "Prints semantic tree",
                    },
                },
            },
            cmdline::CommandLineGroup{
                .Name = "Warnings",
                .Options = {
                    cmdline::CommandLineOption{
                        .Name = "w:disable",
                        .Description = "Disable a specific warning",
                        .ValueName = "warning",
                    },
                },
            },
        },
    };

    if (parser.Parse(argc, argv))
    {
        if (parser.Has("help"))
        {
            parser.PrintHelp();
            return 0;
        }

        if (parser.Has("version"))
        {
            fmt::println("weave-docgen");
            fmt::println("branch: {}", WEAVE_PROJECT_BRANCH);
            fmt::println("hash: {}", WEAVE_PROJECT_HASH);
            return 0;
        }

        if (auto value = parser.GetValue("o:output"))
        {
            fmt::println("output: {}", *value);
        }

        if (auto value = parser.GetValue("o:immediate"))
        {
            fmt::println("immediate: {}", *value);
        }

        for (auto const& value : parser.GetValues("w:disable"))
        {
            fmt::println("disable: {}", value);
        }

        for (auto const& value : parser.GetPositional())
        {
            fmt::println("positional: {}", value);
        }
    }
    else
    {
        fmt::print("Error: {}\n", parser._error);
    }

    return 0;
}
