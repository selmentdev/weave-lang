#include "weave/BugCheck.hxx"

#if __has_include(<stacktrace>)
#include <stacktrace>
#endif

namespace weave::bugcheck
{
    // ReSharper disable once CppDFAConstantFunctionResult
    bool BugcheckFailed(
        std::source_location const& location,
        std::string_view condition)
    {
        return BugcheckFailedArgs(location, condition, "", fmt::make_format_args());
    }

    // ReSharper disable once CppDFAConstantFunctionResult
    bool BugcheckFialed(
        std::source_location const& location,
        std::string_view condition,
        std::string_view message)
    {
        return BugcheckFailedArgs(location, condition, "{}", fmt::make_format_args(message));
    }

    bool BugcheckFailedArgs(
        std::source_location const& location,
        std::string_view condition,
        std::string_view format,
        fmt::format_args args)
    {
        fmt::println(stderr, "=== bugcheck ===");
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
