#include "weave/session/Options.hxx"

namespace weave::session::impl
{
    struct NameValue final
    {
        std::string_view Name;
        std::optional<std::string_view> Value;

        [[nodiscard]] static constexpr NameValue FromString(std::string_view value)
        {
            auto const separator = value.find('=');

            if (separator != std::string_view::npos)
            {
                return NameValue{value.substr(0, separator), value.substr(separator + 1)};
            }

            return NameValue{value, std::nullopt};
        }
    };
}

namespace weave::session
{
    bool FromCommandLine(
        Options& options,
        errors::Handler& handler,
        std::span<std::string_view const> values)
    {
        bool result = true;

        for (auto const& option : values)
        {
            auto const& [name, value] = impl::NameValue::FromString(option);

            if (not options.SetOption(name, value))
            {
                if (value.has_value())
                {
                    handler.AddError(
                        fmt::format("Failed to set option '{}' to value '{}'", name, *value));
                }
                else
                {
                    handler.AddError(
                        fmt::format("Option '{}' requires value", name));
                }

                result = false;
            }
        }

        return result;
    }
}
