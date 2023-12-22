#include "weave/system/Environment.hxx"
#include "weave/bugcheck/BugCheck.hxx"

#include <utility>
#include <cstdlib>

namespace weave::system::environment
{
    bool GetVariable(std::string& result, std::string_view name)
    {
        std::string sname{name};
        
        if (const char* value = secure_getenv(sname.c_str()))
        {
            result = value;
            return true;
        }

        return false;
    }

    std::optional<std::string> GetVariable(std::string_view name)
    {
        std::string result{};

        if (GetVariable(result, name))
        {
            return result;
        }

        return std::nullopt;
    }

    bool SetVariable(std::string_view name, std::string_view value)
    {
        std::string sname{name};
        std::string svalue{value};
        
        return setenv(sname.c_str(), svalue.c_str(), 1) == 0;
    }

    bool RemoveVariable(std::string_view name)
    {
        std::string sname{name};
        
        return unsetenv(sname.c_str()) == 0;
    }
}
