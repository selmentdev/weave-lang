#pragma once
#include "weave/platform/Compiler.hxx"

#include <string_view>
#include <source_location>
#include <utility>

#include <fmt/format.h>

namespace weave::bugcheck
{
    void BugCheck(
        std::source_location const& location,
        std::string_view message);

    void BugCheckArgs(
        std::source_location const& location,
        std::string_view format,
        fmt::format_args args);

    template <typename... Args>
    void BugCheck(
        std::source_location const& location,
        std::string_view format,
        Args const&... args) noexcept
    {
        BugCheckArgs(location, format, fmt::make_format_args(args...));
    }
}

#define WEAVE_BUGCHECK(format, ...) \
    do \
    { \
        ::weave::bugcheck::BugCheck(std::source_location::current(), format __VA_OPT__(, ) __VA_ARGS__); \
        ::std::unreachable(); \
    } while (false)
