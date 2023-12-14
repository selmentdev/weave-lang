#pragma once
#include <string_view>
#include <optional>
#include <array>
#include <span>
#include <charconv>

#include "weave/CommandLine.hxx"
#include "weave/errors/Handler.hxx"

namespace weave::session::impl
{
    constexpr bool ParseBoolean(bool& result, std::optional<std::string_view> const& value)
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
    constexpr bool ParseInteger(T& result, std::optional<std::string_view> const& value)
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
    constexpr bool ParseFloat(T& result, std::optional<std::string_view> const& value)
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
}

namespace weave::session
{
    class Options
    {
    public:
        virtual ~Options() = default;

        virtual bool SetOption(std::string_view name, std::optional<std::string_view> value) = 0;
    };

    bool FromCommandLine(
        Options& options,
        errors::Handler& handler,
        std::span<std::string_view const> values);
}
