#pragma once
#include "weave/platform/Compiler.hxx"

#include <string_view>
#include <source_location>

#include <fmt/format.h>

namespace weave::bugcheck
{
    [[nodiscard]] bool AssertionFailed(
        std::source_location const& location,
        std::string_view condition);

    [[nodiscard]] bool AssertionFailed(
        std::source_location const& location,
        std::string_view condition,
        std::string_view message);

    [[nodiscard]] bool AssertionFailedArgs(
        std::source_location const& location,
        std::string_view condition,
        std::string_view format,
        fmt::format_args args);

    template <typename... Args>
    [[nodiscard]] bool AssertionFailed(
        std::source_location const& location,
        std::string_view condition,
        std::string_view format,
        Args const&... args) noexcept
    {
        return AssertionFailedArgs(location, condition, format, fmt::make_format_args(args...));
    }

    [[nodiscard]] bool BugCheck(
        std::source_location const& location,
        std::string_view message);

    [[nodiscard]] bool BugCheckArgs(
        std::source_location const& location,
        std::string_view format,
        fmt::format_args args);

    template <typename... Args>
    [[nodiscard]] bool BugCheck(
        std::source_location const& location,
        std::string_view format,
        Args const&... args) noexcept
    {
        return BugCheckArgs(location, format, fmt::make_format_args(args...));
    }
}

#define WEAVE_BUGCHECK(format, ...) \
    do \
    { \
        if (::weave::bugcheck::BugCheck(std::source_location::current(), format __VA_OPT__(, ) __VA_ARGS__)) \
        { \
            WEAVE_DEBUGBREAK(); \
        } \
    } while (false)

#define WEAVE_ENSURE(condition, ...) \
    do \
    { \
        if (!(condition)) \
        { \
            if (::weave::bugcheck::AssertionFailed(std::source_location::current(), #condition __VA_OPT__(, ) __VA_ARGS__)) \
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
            if (::weave::bugcheck::AssertionFailed(std::source_location::current(), #condition __VA_OPT__(, ) __VA_ARGS__)) \
            { \
                WEAVE_DEBUGBREAK(); \
            } \
        } \
    } while (false)
#else
#define WEAVE_ASSERT(condition, ...)
#endif
