#pragma once
#include "weave/platform/compiler.hxx"

#include <string_view>
#include <source_location>

#include <fmt/format.h>

namespace weave::bugcheck
{
    [[nodiscard]] bool BugcheckFailed(
        std::source_location const& location,
        std::string_view condition);

    [[nodiscard]] bool BugcheckFialed(
        std::source_location const& location,
        std::string_view condition,
        std::string_view message);

    [[nodiscard]] bool BugcheckFailedArgs(
        std::source_location const& location,
        std::string_view condition,
        std::string_view format,
        fmt::format_args args);

    template <typename... Args>
    [[nodiscard]] bool BugcheckFailed(
        std::source_location const& location,
        std::string_view condition,
        std::string_view format,
        Args const&... args) noexcept
    {
        return BugcheckFailedArgs(location, condition, format, fmt::make_format_args(args...));
    }
}

#define WEAVE_ENSURE(condition, ...) \
    do \
    { \
        if (!(condition)) \
        { \
            if (::weave::bugcheck::BugcheckFailed(std::source_location::current(), #condition __VA_OPT__(, ) __VA_ARGS__)) \
            { \
                WEAVE_DEBUGBREAK(); \
            } \
        } \
    } while (false)

#ifndef NDEBUG
#define WEAVE_ASSERT(condition, ...) \
    do \
    { \
        if (!(condition)) \
        { \
            if (::weave::bugcheck::BugcheckFailed(std::source_location::current(), #condition __VA_OPT__(, ) __VA_ARGS__)) \
            { \
                WEAVE_DEBUGBREAK(); \
            } \
        } \
    } while (false)
#else
#define WEAVE_ASSERT(condition, ...)
#endif
