#include "weave/session/SanitizersSpec.hxx"
#include "weave/core/String.hxx"

namespace weave::session
{
    bool weave::session::SanitizersSpec::Parse(SanitizersSpec& result, std::optional<std::string_view> const& value)
    {
        result = {};

        for (std::string const& item : core::Split(*value, ','))
        {
            if (item == "address")
            {
                result.AddressSanitizer = true;
            }
            else if (item == "thread")
            {
                result.ThreadSanitizer = true;
            }
            else if (item == "memory")
            {
                result.MemorySanitizer = true;
            }
            else if (item == "leak")
            {
                result.MemorySanitizer = true;
            }
            else if (item == "undefined")
            {
                result.UndefinedBehaviorSanitizer = true;
            }
            else
            {
                return false;
            }
        }

        return true;
    }
}
