#include "weave/commandline.hxx"
#include "weave/bugcheck.hxx"

#include <algorithm>

namespace weave::commandline
{
    std::vector<std::string_view> CommandLineParseResult::GetValues(std::string_view name) const
    {
        std::vector<std::string_view> result{};

        for (auto const& [index, value] : this->m_Values)
        {
            if (this->m_Names[index] == name)
            {
                result.emplace_back(value);
            }
        }

        return result;
    }

    std::optional<std::string_view> CommandLineParseResult::GetValue(std::string_view name) const
    {
        for (auto const& [index, value] : this->m_Values)
        {
            if (this->m_Names[index] == name)
            {
                return value;
            }
        }

        return std::nullopt;
    }

    std::span<std::string_view const> CommandLineParseResult::GetPositional() const
    {
        return this->m_Positional;
    }

    bool CommandLineParseResult::HasFlag(std::string_view name) const
    {
        return std::find(this->m_Names.begin(), this->m_Names.end(), name) != this->m_Names.end();
    }
};

namespace weave::commandline
{
    CommandLineOption const* CommandLineBuilder::FindOption(std::string_view name, bool long_name) const
    {
        for (auto const& option : this->m_Options)
        {
            std::string_view const actual = long_name ? option.Name : option.ShortName;

            if (actual == name)
            {
                return &option;
            }
        }

        return nullptr;
    }

    CommandLineBuilder& CommandLineBuilder::Add(std::string_view name, std::string_view shortName, std::string_view description, std::string_view hint, CommandLineOptionArity arity, CommandLineOptionUsage usage) &
    {
        WEAVE_ASSERT(not name.starts_with("-"), "Name must not start with '-'");
        WEAVE_ASSERT(not shortName.starts_with("-"), "Short name must not start with '-'");

        WEAVE_ASSERT(shortName.empty() or (shortName.size() == 1), "Short name must be single digit only");
        WEAVE_ASSERT(not name.empty() or not shortName.empty());

        this->m_Options.push_back(CommandLineOption{
            .Name = name,
            .ShortName = shortName,
            .Description = description,
            .Hint = hint,
            .Arity = arity,
            .Usage = usage,
        });

        return *this;
    }

    CommandLineBuilder& CommandLineBuilder::Flag(std::string_view name, std::string_view shortName, std::string_view description) &
    {
        return this->Add(name, shortName, description, {}, CommandLineOptionArity::None, CommandLineOptionUsage::Optional);
    }

    CommandLineBuilder& CommandLineBuilder::Single(std::string_view name, std::string_view shortName, std::string_view description, std::string_view hint) &
    {
        return this->Add(name, shortName, description, hint, CommandLineOptionArity::Single, CommandLineOptionUsage::Optional);
    }

    CommandLineBuilder& CommandLineBuilder::RequiredSingle(std::string_view name, std::string_view shortName, std::string_view description, std::string_view hint) &
    {
        return this->Add(name, shortName, description, hint, CommandLineOptionArity::Single, CommandLineOptionUsage::Required);
    }

    CommandLineBuilder& CommandLineBuilder::Multiple(std::string_view name, std::string_view shortName, std::string_view description, std::string_view hint) &
    {
        return this->Add(name, shortName, description, hint, CommandLineOptionArity::Multiple, CommandLineOptionUsage::Optional);
    }

    CommandLineBuilder& CommandLineBuilder::RequiredMultiple(std::string_view name, std::string_view shortName, std::string_view description, std::string_view hint) &
    {
        return this->Add(name, shortName, description, hint, CommandLineOptionArity::Multiple, CommandLineOptionUsage::Required);
    }

    std::expected<CommandLineParseResult, CommandLineErrorResult> CommandLineBuilder::Parse(std::span<const char*> args) const&
    {
        std::vector<std::string_view> resultNames{};
        std::vector<std::pair<size_t, std::string_view>> resultValues{};
        std::vector<std::string_view> resultPositional{};

        for (auto it = args.begin(); it != args.end(); ++it)
        {
            std::string_view item{*it};

            if (item == "--")
            {
                // Skip this item.
                ++it;

                // Insert rest of arguments as-is
                resultPositional.insert(resultPositional.end(), it, args.end());
                break;
            }

            if (item.starts_with('-'))
            {
                bool long_name = false;

                // Handle name of the parameter
                item.remove_prefix(1);

                if (item.starts_with('-'))
                {
                    long_name = true;

                    // This is long name. Strip '--'
                    item.remove_prefix(1);

                    if (item.size() <= 1)
                    {
                        return std::unexpected(CommandLineErrorResult{.Error = CommandLineError::InvalidFormat, .Option = item});
                    }
                }
                else
                {
                    // This is short option. Verify if it has just single letter.
                    if (item.empty())
                    {
                        return std::unexpected(CommandLineErrorResult{.Error = CommandLineError::InvalidFormat, .Option = item});
                    }
                }

                size_t const nameValueSeparator = item.find(':');
                std::string_view const name = item.substr(0, nameValueSeparator);

                if (CommandLineOption const* option = this->FindOption(name, long_name); option != nullptr)
                {
                    // Found option. Get or add it to names by index.
                    size_t optionIndex;

                    if (auto optit = std::find(resultNames.begin(), resultNames.end(), option->Name); optit != resultNames.end())
                    {
                        // Verify if option can be set multiple times.
                        if (option->Arity != CommandLineOptionArity::Multiple)
                        {
                            return std::unexpected(CommandLineErrorResult{.Error = CommandLineError::DuplicatedOption, .Option = name});
                        }

                        optionIndex = std::distance(resultNames.begin(), optit);
                    }
                    else
                    {
                        optionIndex = resultNames.size();
                        resultNames.emplace_back(option->Name);
                    }

                    if (option->Arity != CommandLineOptionArity::None)
                    {
                        if (nameValueSeparator != std::string_view::npos)
                        {
                            resultValues.emplace_back(optionIndex, item.substr(nameValueSeparator + 1));
                        }
                        else
                        {
                            ++it;

                            if (it == args.end())
                            {
                                return std::unexpected(CommandLineErrorResult{.Error = CommandLineError::MissingValue, .Option = name});
                            }

                            resultValues.emplace_back(optionIndex, *it);
                        }
                    }
                    else
                    {
                        // Arity is none - validate if '=' was after the option.
                        if (nameValueSeparator != std::string_view::npos)
                        {
                            return std::unexpected(CommandLineErrorResult{.Error = CommandLineError::UnrecognizedValue, .Option = name});
                        }
                    }
                }
                else
                {
                    return std::unexpected(CommandLineErrorResult{.Error = CommandLineError::UnrecognizedOption, .Option = name});
                }
            }
            else
            {
                // Just a positional parameter. Put it back.
                resultPositional.emplace_back(*it);
            }
        }

        // Verify if all required options are present.
        for (auto const& option : this->m_Options)
        {
            if (option.Usage == CommandLineOptionUsage::Required)
            {
                if (std::find(resultNames.begin(), resultNames.end(), option.Name) == resultNames.end())
                {
                    if (not option.Name.empty())
                    {
                        return std::unexpected(CommandLineErrorResult{.Error = CommandLineError::MissingOption, .Option = option.Name});
                    }

                    return std::unexpected(CommandLineErrorResult{.Error = CommandLineError::MissingOption, .Option = option.ShortName});
                }
            }
        }

        return CommandLineParseResult{
            std::move(resultNames),
            std::move(resultValues),
            std::move(resultPositional),
        };
    }
}

namespace weave::commandline
{
    std::string format_option(CommandLineOption const& option)
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
