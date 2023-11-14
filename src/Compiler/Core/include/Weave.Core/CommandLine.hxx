#pragma once
#include <string_view>
#include <vector>
#include <optional>
#include <expected>
#include <span>

#include <fmt/format.h>

namespace Weave
{
    enum class CommandLineOptionArity
    {
        None,
        Single,
        Multiple,
    };

    enum class CommandLineOptionUsage
    {
        Required,
        Optional,
    };

    struct CommandLineOption
    {
        std::string_view Name{};
        std::string_view ShortName{};
        std::string_view Description{};
        std::string_view Hint{};
        CommandLineOptionArity Arity{};
        CommandLineOptionUsage Usage{};
    };

    class CommandLineBuilder;

    class CommandLineParseResult
    {
        friend class CommandLineBuilder;

    private:
        std::vector<std::string_view> m_Names{};
        std::vector<std::pair<size_t, std::string_view>> m_Values{};
        std::vector<std::string_view> m_Positional{};

    private:
        CommandLineParseResult(
            std::vector<std::string_view>&& names,
            std::vector<std::pair<size_t, std::string_view>>&& values,
            std::vector<std::string_view>&& positional)
            : m_Names{names}
            , m_Values{values}
            , m_Positional{positional}
        {
        }

    public:
        std::vector<std::string_view> GetValues(std::string_view name) const;

        std::optional<std::string_view> GetValue(std::string_view name) const;

        std::span<std::string_view const> GetPositional() const;

        bool HasFlag(std::string_view name) const;
    };

    enum class CommandLineError
    {
        InvalidFormat,
        MissingOption,
        MissingValue,
        DuplicatedOption,
        UnrecognizedOption,
        UnrecognizedValue,
    };

    struct CommandLineErrorResult final
    {
        CommandLineError Error{};
        std::string_view Option{};
    };

    class CommandLineBuilder final
    {
    private:
        std::vector<CommandLineOption> m_Options{};

        CommandLineOption const* FindOption(std::string_view name) const;

    public:
        std::span<CommandLineOption const> GetOptions() const&
        {
            return this->m_Options;
        }

        CommandLineBuilder& Add(std::string_view name, std::string_view shortName, std::string_view description, std::string_view hint, CommandLineOptionArity arity, CommandLineOptionUsage usage) &;

        CommandLineBuilder& Flag(std::string_view name, std::string_view shortName, std::string_view description) &;

        CommandLineBuilder& Single(std::string_view name, std::string_view shortName, std::string_view description, std::string_view hint) &;

        CommandLineBuilder& Single(std::string_view name, std::string_view shortName, std::string_view description) &
        {
            return this->Single(name, shortName, description, {});
        }

        CommandLineBuilder& Single(std::string_view name, std::string_view shortName) &
        {
            return this->Single(name, shortName, {}, {});
        }

        CommandLineBuilder& RequiredSingle(std::string_view name, std::string_view shortName, std::string_view description, std::string_view hint) &;

        CommandLineBuilder& Multiple(std::string_view name, std::string_view shortName, std::string_view description, std::string_view hint) &;

        CommandLineBuilder& RequiredMultiple(std::string_view name, std::string_view shortName, std::string_view description, std::string_view hint) &;

        [[nodiscard]] std::expected<CommandLineParseResult, CommandLineErrorResult> Parse(std::span<const char*> args) const&;
    };

    constexpr std::string FormatOption(CommandLineOption const& option)
    {
        std::string line{};

        if (option.Usage != CommandLineOptionUsage::Required)
        {
            line.push_back('[');
        }

        bool const has_short_name = !option.ShortName.empty();
        bool const has_long_name = !option.Name.empty();

        if (has_short_name)
        {
            line.push_back('-');
            line.append(option.ShortName);
        }

        if (has_long_name)
        {
            if (has_short_name)
            {
                line.append(", ");
            }

            line.append("--");
            line.append(option.Name);
        }

        if (option.Arity != CommandLineOptionArity::None)
        {
            line.push_back(' ');
            line.append(option.Hint);
        }

        if (option.Usage != CommandLineOptionUsage::Required)
        {
            line.push_back(']');
        }

        if (option.Arity == CommandLineOptionArity::Multiple)
        {
            line.append("..");
        }

        return line;
    }
}

template <>
struct fmt::formatter<Weave::CommandLineError>
    : formatter<std::string_view>
{
    constexpr auto format(Weave::CommandLineError const& value, auto& context)
    {
        std::string_view result = "unknown";

        switch (value)
        {
        case Weave::CommandLineError::MissingValue:
            result = "missing value";
            break;
        case Weave::CommandLineError::DuplicatedOption:
            result = "duplicated option";
            break;
        case Weave::CommandLineError::InvalidFormat:
            result = "invalid format";
            break;
        case Weave::CommandLineError::MissingOption:
            result = "missing option";
            break;
        case Weave::CommandLineError::UnrecognizedOption:
            result = "unrecognized option";
            break;
        case Weave::CommandLineError::UnrecognizedValue:
            result = "unrecognized value";
            break;
        }

        return formatter<std::string_view>::format(result, context);
    }
};

template <>
struct fmt::formatter<Weave::CommandLineErrorResult>
{
    constexpr auto parse(auto& context)
    {
        return context.begin();
    }

    constexpr auto format(Weave::CommandLineErrorResult const& value, auto& context)
    {
        return fmt::format_to(context.out(), "{}: {}", value.Error, value.Option);
    }
};
