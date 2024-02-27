#include "weave/commandline/CommandLineParser.hxx"

#include <charconv>

#include <fmt/format.h>

namespace weave::commandline
{
    std::optional<uint32_t> TryParseUInt32(
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

    std::optional<int32_t> TryParseInt32(
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

    std::optional<uint64_t> TryParseUInt64(
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

    std::optional<int64_t> TryParseInt64(
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

    std::optional<double> TryParseFloat64(
        std::optional<std::string_view> const& value)
    {
        if (value)
        {
            double result;

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

    std::optional<bool> TryParseBool(
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

    std::optional<std::string_view> TryParseString(
        std::optional<std::string_view> const& value)
    {
        return value;
    }

    std::optional<std::string_view> TryParseFilePath(
        std::optional<std::string_view> const& value)
    {
        return value;
    }
}
