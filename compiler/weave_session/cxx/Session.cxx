#include "weave/Session.hxx"

#include <array>
#include <span>

namespace weave::session::impl
{
    std::optional<std::string_view> SplitNameValue(std::string_view value, std::string_view& name)
    {
        auto const separator = value.find('=');

        if (separator != std::string_view::npos)
        {
            name = value.substr(0, separator);
            return value.substr(separator + 1);
        }
        else
        {
            name = value;
            return std::nullopt;
        }
    }

    bool ParseBoolean(bool& result, std::optional<std::string_view> const& value)
    {
        if (value.has_value())
        {
            if (*value == "true" || *value == "yes" || *value == "on")
            {
                result = true;
                return true;
            }

            if (*value == "false" || *value == "no" || *value == "off")
            {
                result = false;
                return true;
            }
        }

        return false;
    }


}

namespace weave::session
{
}

namespace weave::session
{
    template <typename O>
    using OptionSetter = bool(O&, std::optional<std::string_view> const&);

    template <typename O>
    struct Option
    {
        std::string_view name;
        std::string_view description;
        OptionSetter<O>* setter;
    };

    static constexpr Option<CodeGeneratorOptions> g_CodeGeneratorOptions[]{
        // clang-format off
        {"checked", "Enables checked build", [](CodeGeneratorOptions& self, std::optional<std::string_view> const& value) -> bool { return impl::ParseBoolean(self.Checked, value); }},
        {"debug", "Enables debug symbol info", [](CodeGeneratorOptions& self, std::optional<std::string_view> const& value) -> bool { return impl::ParseBoolean(self.Debug, value); }},
        // clang-format on
    };

    template <typename O>
    bool ApplyOption(std::span<Option<O> const> options, O& result, std::string_view name, std::optional<std::string_view> value)
    {
        auto it = std::find_if(options.begin(), options.end(), [&](Option<O> const& option)
            {
                return option.name == name;
            });

        if (it != options.end())
        {
            return it->setter(result, value);
        }

        return false;
    }

    CodeGeneratorOptions CodeGeneratorOptions::FromCommandLine(
        errors::Handler& handler,
        commandline::CommandLineParseResult const& command_line)
    {
        CodeGeneratorOptions result{};

        for (auto const& item : command_line.GetValues("codegen"))
        {
            std::string_view name{};
            auto const& value = impl::SplitNameValue(item, name);

            if (!ApplyOption(std::span<Option<CodeGeneratorOptions> const>{g_CodeGeneratorOptions}, result, name, value))
            {
                if (value.has_value())
                {
                    handler.AddError(fmt::format("Failed to set option '{}' to value '{}'", name, *value));
                }
                else
                {
                    handler.AddError(fmt::format("Option '{}' requires value", name));
                }
            }
        }

        return result;
    }


    ExperimentalOptions ExperimentalOptions::FromCommandLine(
        errors::Handler& handler,
        commandline::CommandLineParseResult const& command_line)
    {
        (void)handler;
        (void)command_line;
        return {};
    }
}
