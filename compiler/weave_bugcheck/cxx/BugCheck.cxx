#include "weave/BugCheck.hxx"

#if __has_include(<stacktrace>)
#include <stacktrace>
#endif

namespace weave::bugcheck
{
    // ReSharper disable once CppDFAConstantFunctionResult
    bool AssertionFailed(
        std::source_location const& location,
        std::string_view condition)
    {
        return AssertionFailedArgs(location, condition, "", fmt::make_format_args());
    }

    // ReSharper disable once CppDFAConstantFunctionResult
    bool AssertionFailed(
        std::source_location const& location,
        std::string_view condition,
        std::string_view message)
    {
        return AssertionFailedArgs(location, condition, "{}", fmt::make_format_args(message));
    }

    bool AssertionFailedArgs(
        std::source_location const& location,
        std::string_view condition,
        std::string_view format,
        fmt::format_args args)
    {
        fmt::println(stderr, "=== assert ===");
        fmt::println(stderr, "{}:{}:{}: {}", location.file_name(), location.line(), location.column(), condition);

        if (not format.empty())
        {
            fmt::print(stderr, "\"");
            fmt::vprint(stderr, fmt::string_view{format.data(), format.size()}, args);
            fmt::println(stderr, "\"");
        }

#if defined(__cpp_lib_stacktrace)
        fmt::println(stderr, "stacktrace:");

        for (std::stacktrace_entry const& entry : std::stacktrace::current())
        {
            fmt::println(stderr, "{}", std::to_string(entry));
        }
#else
        fmt::println(stderr, "stacktrace: not available");
#endif


#if !defined(NDEBUG)
        return true;
#else
        std::abort();
#endif
    }
}

namespace weave::bugcheck
{
    bool BugCheck(
        std::source_location const& location,
        std::string_view message)
    {
        return BugCheckArgs(location, "{}", fmt::make_format_args(message));
    }

    bool BugCheckArgs(
        std::source_location const& location,
        std::string_view format,
        fmt::format_args args)
    {
        fmt::println(stderr, "=== bugcheck ===");
        fmt::println(stderr, "{}:{}:{}", location.file_name(), location.line(), location.column());

        if (not format.empty())
        {
            fmt::print(stderr, "\"");
            fmt::vprint(stderr, fmt::string_view{format.data(), format.size()}, args);
            fmt::println(stderr, "\"");
        }

#if defined(__cpp_lib_stacktrace)
        fmt::println(stderr, "stacktrace:");

        for (std::stacktrace_entry const& entry : std::stacktrace::current())
        {
            fmt::println(stderr, "{}", std::to_string(entry));
        }
#else
        fmt::println(stderr, "stacktrace: not available");
#endif


#if !defined(NDEBUG)
        return true;
#else
        std::abort();
#endif
    }
}
