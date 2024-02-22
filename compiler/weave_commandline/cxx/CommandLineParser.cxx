#include "weave/commandline/CommandLineParser.hxx"

#include <charconv>
#include <vector>

#include <fmt/format.h>

namespace weave::commandline
{
    bool CommandLineParser::TryParseOption(
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
                value = "+";
            }
            else if (argument.ends_with('-'))
            {
                name = argument.substr(0, argument.size() - 1);
                value = "-";
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

    std::optional<uint32_t> CommandLineParser::TryParseUInt32(
        std::optional<std::string_view> const& value)
    {
        if (value)
        {
            int radix = 10;

            uint32_t result;

            auto [ptr, ec] = std::from_chars(
                value->data(),
                value->data() + value->size(),
                result,
                radix);

            if (ec == std::errc())
            {
                return result;
            }
        }

        return std::nullopt;
    }

    std::optional<int32_t> CommandLineParser::TryParseInt32(
        std::optional<std::string_view> const& value)
    {
        if (value)
        {
            int32_t result;

            auto [ptr, ec] = std::from_chars(
                value->data(),
                value->data() + value->size(),
                result);

            if (ec == std::errc())
            {
                return result;
            }
        }

        return std::nullopt;
    }

    std::optional<uint64_t> CommandLineParser::TryParseUInt64(
        std::optional<std::string_view> const& value)
    {
        if (value)
        {
            int radix = 10;

            uint64_t result;

            auto [ptr, ec] = std::from_chars(
                value->data(),
                value->data() + value->size(),
                result,
                radix);

            if (ec == std::errc())
            {
                return result;
            }
        }

        return std::nullopt;
    }

    std::optional<int64_t> CommandLineParser::TryParseInt64(
        std::optional<std::string_view> const& value)
    {
        if (value)
        {
            int64_t result;

            auto [ptr, ec] = std::from_chars(
                value->data(),
                value->data() + value->size(),
                result);

            if (ec == std::errc())
            {
                return result;
            }
        }

        return std::nullopt;
    }

    std::optional<bool> CommandLineParser::TryParseBool(
        std::optional<std::string_view> const& value)
    {
        if (value)
        {
            if ((value == "true") or (value == "yes") or (value == "on") or (value == "1") or (value == "+"))
            {
                return true;
            }

            if ((value == "false") or (value == "no") or (value == "off") or (value == "0") or (value == "-"))
            {
                return false;
            }
        }

        return std::nullopt;
    }

    std::optional<std::string_view> CommandLineParser::TryParseString(
        std::optional<std::string_view> const& value)
    {
        return value;
    }

    std::optional<std::string_view> CommandLineParser::TryParseFilePath(
        std::optional<std::string_view> const& value)
    {
        return value;
    }

    bool CommandLineParser::Parse(int argc, char** argv)
    {
        bool parseOptions = true;

        for (int current = 1; current < argc; ++current)
        {
            std::string_view argument = argv[current];

            if (parseOptions and argument.starts_with('-'))
            {
                if (argument != "--")
                {
                    std::string_view name{};
                    std::optional<std::string_view> value{};

                    if (TryParseOption(argument, name, value))
                    {
                        if (this->OnOption(name, value))
                        {
                            continue;
                        }
                    }

                    fmt::println("unrecognized option '{}'", argument);
                    return false;
                }

                parseOptions = false;
            }
            else
            {
                this->OnPositionalArgument(argument);
            }
        }

        return true;
    }
}
