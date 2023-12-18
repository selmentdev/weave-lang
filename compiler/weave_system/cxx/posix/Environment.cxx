#include "weave/system/Environment.hxx"

namespace weave::system::environment
{
    bool GetVariable(std::string& result, std::string_view name)
    {
        (void)result;
        (void)name;
        std::unreachable();
    }

    std::optional<std::string> GetVariable(std::string_view name)
    {
        (void)name;
        std::unreachable();
    }

    bool SetVariable(std::string_view name, std::string_view value)
    {
        (void)name;
        (void)value;
        std::unreachable();
    }

    bool RemoveVariable(std::string_view name)
    {
        (void)name;
        std::unreachable();
    }
}
