#include "Compiler.Core/Assert.hxx"

namespace Weave
{
    [[nodiscard]] bool AssertionFailed(
        std::source_location const& location,
        std::string_view condition) noexcept
    {
        return AssertionFailedArgs(location, condition, "", fmt::make_format_args());
    }

    [[nodiscard]] bool AssertionFailed(
        std::source_location const& location,
        std::string_view condition,
        std::string_view message) noexcept
    {
        return AssertionFailedArgs(location, condition, "{}", fmt::make_format_args(message));
    }

    [[nodiscard]] bool AssertionFailedArgs(
        std::source_location const& location,
        std::string_view condition,
        std::string_view format,
        fmt::format_args args) noexcept
    {
        fmt::println("{}({}): assertion failed: {}", location.file_name(), location.line(), condition);

        fmt::print("message: ");
        fmt::vprint(format, args);
        fmt::println("");

#if !defined(NDEBUG)
        std::abort();
#else
        return true;
#endif
    }
}
