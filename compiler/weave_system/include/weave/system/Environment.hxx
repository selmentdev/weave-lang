#pragma once
#include <optional>
#include <string_view>
#include <string>

namespace weave::system::environment
{
    bool GetVariable(std::string& result, std::string_view name);

    std::optional<std::string> GetVariable(std::string_view name);

    bool SetVariable(std::string_view name, std::string_view value);

    bool RemoveVariable(std::string_view name);
}
