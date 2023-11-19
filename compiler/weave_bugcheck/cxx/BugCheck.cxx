#include "weave/bugcheck.hxx"

namespace weave::bugcheck
{
    bool BugcheckFailed(
        std::source_location const& location,
        std::string_view condition)
    {
        return BugcheckFailedArgs(location, condition, "<none>", fmt::make_format_args());
    }

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
        fmt::println(stderr, "{}({}): assertion failed: {}", location.file_name(), location.line(), condition);

        fmt::print(stderr, "message: \"");
        fmt::vprint(stderr, fmt::string_view{format.data(), format.size()}, args);
        fmt::println(stderr, "\"");

#if !defined(NDEBUG)
        return true;
#else
        std::abort();
#endif
    }
}
