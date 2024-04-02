#pragma once
#include <expected>
#include <span>
#include <string_view>
#include <optional>
#include <vector>
#include <algorithm>

#include <fmt/format.h>

namespace weave::commandline
{
    std::optional<uint32_t> TryParseUInt32(
        std::optional<std::string_view> const& value);

    std::optional<int32_t> TryParseInt32(
        std::optional<std::string_view> const& value);

    std::optional<uint64_t> TryParseUInt64(
        std::optional<std::string_view> const& value);

    std::optional<int64_t> TryParseInt64(
        std::optional<std::string_view> const& value);

    std::optional<double> TryParseFloat64(
        std::optional<std::string_view> const& value);

    std::optional<bool> TryParseBool(
        std::optional<std::string_view> const& value);

    std::optional<std::string_view> TryParseString(
        std::optional<std::string_view> const& value);

    std::optional<std::string_view> TryParseFilePath(
        std::optional<std::string_view> const& value);
}

namespace weave::commandline
{
    class ArgumentEnumerator final
    {
    private:
        char** _current{};
        char** _first{};
        char** _last{};

    public:
        ArgumentEnumerator(int argc, char** argv) noexcept
            : _current(argv + 1)
            , _first(argv + 1)
            , _last(argv + argc)
        {
        }

        [[nodiscard]] bool HasNext() const
        {
            return _current != _last;
        }

        [[nodiscard]] std::optional<std::string_view> Next()
        {
            if (HasNext())
            {
                return std::string_view(*_current++);
            }

            return std::nullopt;
        }
    };
}

namespace weave::commandline
{
    class ArgumentParser;

    struct Option final
    {
        std::string_view Name{};
        std::string_view Description{};
        std::string_view ValueName{};
        std::string_view DefaultValue{};
    };

    struct Command final
    {
        std::string_view Name{};
        std::string_view Description{};
    };

    struct Argument final
    {
        std::string_view Name{};
        std::string_view Description{};
    };

    class ArgumentParseResult final
    {
        friend class ArgumentParser;

    private:
        std::optional<std::string_view> _command{};
        std::vector<std::string_view> _names{};
        std::vector<std::pair<std::size_t, std::string_view>> _values{};
        std::vector<std::string_view> _arguments{};

    public:
        [[nodiscard]] std::optional<std::string_view> GetCommand() const
        {
            return this->_command;
        }

        [[nodiscard]] bool Contains(std::string_view name) const
        {
            return std::find(_names.cbegin(), _names.cend(), name) != _names.cend();
        }

        [[nodiscard]] bool Contains(Option const& option) const
        {
            return std::find(_names.cbegin(), _names.cend(), option.Name) != _names.cend();
        }

        [[nodiscard]] std::optional<std::string_view> GetValue(std::string_view name) const
        {
            auto it = std::find(_names.cbegin(), _names.cend(), name);

            if (it != _names.cend())
            {
                auto index = std::distance(_names.cbegin(), it);
                return _values[index].second;
            }

            return std::nullopt;
        }

        [[nodiscard]] std::optional<std::string_view> GetValue(Option const& option) const
        {
            auto it = std::find(_names.cbegin(), _names.cend(), option.Name);

            if (it != _names.cend())
            {
                auto index = std::distance(_names.cbegin(), it);
                return _values[index].second;
            }

            return std::nullopt;
        }

        [[nodiscard]] std::optional<std::string_view> First(std::string_view name) const
        {
            auto it = std::find(_names.cbegin(), _names.cend(), name);

            if (it != _names.cend())
            {
                auto index = std::distance(_names.cbegin(), it);
                return _values[index].second;
            }

            return std::nullopt;
        }

        [[nodiscard]] std::optional<std::string_view> First(Option const& option) const
        {
            auto it = std::find(_names.cbegin(), _names.cend(), option.Name);

            if (it != _names.cend())
            {
                auto index = std::distance(_names.cbegin(), it);
                return _values[index].second;
            }

            return std::nullopt;
        }

        [[nodiscard]] std::optional<std::string_view> Last(std::string_view name) const
        {
            auto it = std::find(_names.crbegin(), _names.crend(), name);

            if (it != _names.crend())
            {
                auto index = std::distance(_names.crbegin(), it);
                return _values[index].second;
            }

            return std::nullopt;
        }

        [[nodiscard]] std::optional<std::string_view> Last(Option const& option) const
        {
            auto it = std::find(_names.crbegin(), _names.crend(), option.Name);

            if (it != _names.crend())
            {
                auto index = std::distance(_names.crbegin(), it);
                return _values[index].second;
            }

            return std::nullopt;
        }

        [[nodiscard]] std::optional<std::string_view> Single(std::string_view name) const
        {
            auto it = std::find(_names.cbegin(), _names.cend(), name);

            if (it != _names.cend())
            {
                auto nt = std::find(std::next(it), _names.cend(), name);

                if (nt == _names.cend())
                {
                    auto index = std::distance(_names.cbegin(), it);
                    return _values[index].second;
                }
            }

            return std::nullopt;
        }

        [[nodiscard]] std::optional<std::string_view> Single(Option const& option) const
        {
            auto it = std::find(_names.cbegin(), _names.cend(), option.Name);

            if (it != _names.cend())
            {
                auto nt = std::find(std::next(it), _names.cend(), option.Name);

                if (nt == _names.cend())
                {
                    auto index = std::distance(_names.cbegin(), it);
                    return _values[index].second;
                }
            }

            return std::nullopt;
        }

        [[nodiscard]] std::string_view SingleOrDefault(std::string_view name, std::string_view defaultValue) const
        {
            if (auto value = Single(name))
            {
                return *value;
            }

            return defaultValue;
        }

        [[nodiscard]] std::string_view SingleOrDefault(Option const& option, std::string_view defaultValue) const
        {
            if (auto value = Single(option))
            {
                return *value;
            }

            return defaultValue;
        }

        [[nodiscard]] std::vector<std::string_view> Multiple(std::string_view name) const
        {
            std::vector<std::string_view> values{};

            for (const auto& [index, value] : _values)
            {
                if (_names[index] == name)
                {
                    values.push_back(value);
                }
            }

            return values;
        }

        [[nodiscard]] std::vector<std::string_view> Multiple(Option const& option) const
        {
            std::vector<std::string_view> values{};

            for (const auto& [index, value] : _values)
            {
                if (_names[index] == option.Name)
                {
                    values.push_back(value);
                }
            }

            return values;
        }

        [[nodiscard]] std::vector<std::string_view> const& GetPositional() const
        {
            return _arguments;
        }
    };

    enum class ArgumentParseErrorReason
    {
        UnknownOption,
        MissingValue,
        InvalidValue,
        UnknownCommand,
        MissingCommand,
        InvalidCommand,
        UnexpectedArgument,
        MissingArgument,
    };

    struct ArgumentParseError final
    {
        ArgumentParseErrorReason Reason{};
        std::string_view Argument{};
    };

    class ArgumentParser
    {
    private:
        [[nodiscard]] static constexpr bool IsPositional(std::string_view argument)
        {
            return not argument.starts_with('-');
        }

        std::optional<std::string_view> SplitArgument(std::string_view argument, std::string_view& name)
        {
            std::string_view::size_type const separator = argument.find('=');

            if (separator != std::string_view::npos)
            {
                name = argument.substr(0, separator);
                return argument.substr(separator + 1);
            }

            name = argument;
            return std::nullopt;
        }

    private:
        std::vector<Option> _options{};
        std::vector<Argument> _arguments{};
        std::vector<Command> _commands{};

    private:
        [[nodiscard]] Option const* FindOption(std::string_view name) const
        {
            for (Option const& option : this->_options)
            {
                if (option.Name == name)
                {
                    return &option;
                }
            }

            return nullptr;
        }

        [[nodiscard]] Command const* FindCommand(std::string_view name) const
        {
            for (Command const& command : this->_commands)
            {
                if (command.Name == name)
                {
                    return &command;
                }
            }

            return nullptr;
        }

    public:
        void AddOption(Option const& option)
        {
            this->_options.emplace_back(option);
        }

        void AddOption(std::string_view name, std::string_view description)
        {
            this->_options.emplace_back(name, description);
        }

        void AddOption(std::string_view name, std::string_view description, std::string_view valueName)
        {
            this->_options.emplace_back(name, description, valueName);
        }

        void AddOption(std::string_view name, std::string_view description, std::string_view valueName, std::string_view defaultValue)
        {
            this->_options.emplace_back(name, description, valueName, defaultValue);
        }

        void AddArgument(Argument const& argument)
        {
            this->_arguments.emplace_back(argument);
        }

        void AddArgument(std::string_view name, std::string_view description)
        {
            this->_arguments.emplace_back(name, description);
        }

        void AddCommand(std::string_view name, std::string_view description)
        {
            this->_commands.emplace_back(name, description);
        }

        void AddCommand(Command const& command)
        {
            this->_commands.emplace_back(command);
        }

        [[nodiscard]] auto Parse(ArgumentEnumerator& enumerator) const -> std::expected<ArgumentParseResult, ArgumentParseError>
        {
            ArgumentParseResult result{};

            bool parseOptions = true;

            while (std::optional<std::string_view> argument = enumerator.Next())
            {
                if (IsPositional(*argument) or not parseOptions)
                {
                    // FIXME: This could be simplified?
                    if (this->_commands.empty())
                    {
                        result._arguments.push_back(*argument);
                    }
                    else
                    {
                        if (result._command.has_value())
                        {
                            // Command already parsed.
                            result._arguments.push_back(*argument);
                        }
                        else
                        {
                            // Try to handle argument as command.
                            if (this->FindCommand(*argument) != nullptr)
                            {
                                result._command = *argument;
                                break;
                            }
                            else
                            {
                                return std::unexpected(ArgumentParseError{ArgumentParseErrorReason::UnknownCommand, *argument});
                            }
                        }
                    }
                }
                else if (*argument == "--")
                {
                    parseOptions = false;
                }
                else
                {
                    if (Option const* option = this->FindOption(*argument))
                    {
                        if (option->ValueName.empty())
                        {
                            result._names.push_back(*argument);
                        }
                        else
                        {
                            if (std::optional<std::string_view> value = enumerator.Next())
                            {
                                result._names.emplace_back(*argument);
                                result._values.emplace_back(result._names.size() - 1, *value);
                            }
                            else
                            {
                                return std::unexpected(ArgumentParseError{ArgumentParseErrorReason::MissingValue, *argument});
                            }
                        }
                    }
                    else
                    {
                        return std::unexpected(ArgumentParseError{ArgumentParseErrorReason::UnknownOption, *argument});
                    }
                }
            }

            if (not this->_arguments.empty())
            {
                if (result._arguments.size() < this->_arguments.size())
                {
                    return std::unexpected<ArgumentParseError>(ArgumentParseErrorReason::MissingArgument);
                }
            }

            return result;
        }

        void PrintUsage(std::string_view name)
        {
            fmt::print("Usage: {} [command] [options] [arguments]\n", name);
            fmt::print("\n");
            this->PrintHelp();
        }

        void PrintHelp()
        {
            if (not this->_arguments.empty())
            {
                fmt::println("Arguments:");

                for (Argument const& argument : this->_arguments)
                {
                    fmt::print("  {:28}", argument.Name);

                    if (not argument.Description.empty())
                    {
                        fmt::print(" {}", argument.Description);
                    }

                    fmt::println("");
                }

                fmt::println("");
            }

            if (not this->_options.empty())
            {
                fmt::println("Options:");

                for (Option const& option : this->_options)
                {
                    if (option.ValueName.empty())
                    {
                        fmt::print("  {:28}", option.Name);
                    }
                    else
                    {
                        fmt::print("  {:28}", fmt::format("{}=<{}>", option.Name, option.ValueName));
                    }

                    if (not option.Description.empty())
                    {
                        fmt::print(" {}", option.Description);
                    }

                    fmt::println("");
                }

                fmt::println("");
            }

            if (not this->_commands.empty())
            {
                fmt::println("Commands:");

                for (Command const& command : this->_commands)
                {
                    fmt::print("  {:28}", command.Name);

                    if (not command.Description.empty())
                    {
                        fmt::print(" {}", command.Description);
                    }

                    fmt::println("");
                }

                fmt::println("");
            }
        }
    };
}
