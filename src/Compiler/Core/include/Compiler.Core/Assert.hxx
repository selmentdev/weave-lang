#pragma once
#include "Compiler.Core/Platform/Compiler.hxx"

#include <string_view>
#include <source_location>

#include <fmt/format.h>

namespace Weave
{
    [[nodiscard]] bool AssertionFailed(
        std::source_location const& location,
        std::string_view condition) noexcept;

    [[nodiscard]] bool AssertionFailed(
        std::source_location const& location,
        std::string_view condition,
        std::string_view message) noexcept;

    [[nodiscard]] bool AssertionFailedArgs(
        std::source_location const& location,
        std::string_view condition,
        std::string_view format,
        fmt::format_args args) noexcept;

    template <typename... Args>
    [[nodiscard]] bool AssertionFailed(
        std::source_location const& location,
        std::string_view condition,
        std::string_view format,
        Args const&... args) noexcept
    {
        return AssertionFailedArgs(location, condition, format, fmt::make_format_args(args...));
    }
}

#define WEAVE_ENSURE(condition, ...) \
    do \
    { \
        if (!(condition)) \
        { \
            if (::Weave::AssertionFailed(std::source_location::current(), #condition __VA_OPT__(, ) __VA_ARGS__)) \
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
            if (::Weave::AssertionFailed(std::source_location::current(), #condition __VA_OPT__(, ) __VA_ARGS__)) \
            { \
                WEAVE_DEBUGBREAK(); \
            } \
        } \
    } while (false)
#else
#define WEAVE_ASSERT(condition, ...)
#endif

#define ANEMONE_ASSERT(condition, ...) \
    do \
    { \
        if (!(condition)) \
        { \
            if (::Weave::AssertionFailed(std::source_location::current(), #condition __VA_OPT__(, ) __VA_ARGS__)) \
            { \
            } \
        } \
    } while (false);
