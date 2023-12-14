#include "weave/session/ExperimentalOptions.hxx"

namespace weave::session
{
    bool ExperimentalOptions::SetOption(std::string_view name, std::optional<std::string_view> value)
    {
        (void)name;
        (void)value;
        return false;
    }
}
