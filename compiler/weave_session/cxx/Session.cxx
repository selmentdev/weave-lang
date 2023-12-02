#include "weave/Session.hxx"

#include <array>
#include <span>
#include <charconv>
#include "weave/core/String.hxx"

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
}

namespace weave::session::impl
{
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

    template <typename T>
    bool ParseInteger(T& result, std::optional<std::string_view> const& value)
        requires(std::is_integral_v<T>)
    {
        if (value.has_value())
        {
            const char* first = value->data();
            const char* last = first + value->size();

            auto [end, err] = std::from_chars(first, last, result);

            return (end == last) and (err == std::errc{});
        }

        return false;
    }

    template <typename T>
    bool ParseFloat(T& result, std::optional<std::string_view> const& value)
        requires(std::is_floating_point_v<T>)
    {
        if (value.has_value())
        {
            const char* first = value->data();
            const char* last = first + value->size();

            auto [end, err] = std::from_chars(first, last, result);

            return (end == last) and (err == std::errc{});
        }

        return false;
    }

    bool ParseSanitizersSpec(SanitizersSpec& result, std::optional<std::string_view> const& value)
    {
        result = {};

        for (std::string const& item : core::Split(*value, ','))
        {
            if (item == "address")
            {
                result.AddressSanitizer = true;
            }
            else if (item == "thread")
            {
                result.ThreadSanitizer = true;
            }
            else if (item == "memory")
            {
                result.MemorySanitizer = true;
            }
            else if (item == "leak")
            {
                result.MemorySanitizer = true;
            }
            else
            {
                return false;
            }
        }

        return true;
    }
}

namespace weave::session
{
    bool CodeGeneratorOptions::ApplyOptionFromCommandLine(std::string_view name, std::optional<std::string_view> const& value)
    {
        if (name == "debug")
        {
            return impl::ParseBoolean(this->Debug, value);
        }

        if (name == "checked")
        {
            return impl::ParseBoolean(this->Checked, value);
        }

        if (name == "opt")
        {
            return impl::ParseInteger(this->OptimizationLevel, value);
        }

        if (name == "sanitizers")
        {
            return impl::ParseSanitizersSpec(this->Sanitizers, value);
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

            if (not result.ApplyOptionFromCommandLine(name, value))
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


    void CodeGeneratorOptions::DebugPrint()
    {
        fmt::println(stderr, "Checked                                 : {}", this->Checked);
        fmt::println(stderr, "Debug                                   : {}", this->Debug);
        fmt::println(stderr, "OptimizationLevel                       : {}", this->OptimizationLevel);
        fmt::println(stderr, "Sanitizers.AddressSanitizer             : {}", this->Sanitizers.AddressSanitizer);
        fmt::println(stderr, "Sanitizers.ThreadSanitizer              : {}", this->Sanitizers.ThreadSanitizer);
        fmt::println(stderr, "Sanitizers.MemorySanitizer              : {}", this->Sanitizers.MemorySanitizer);
        fmt::println(stderr, "Sanitizers.LeakSanitizer                : {}", this->Sanitizers.LeakSanitizer);
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
