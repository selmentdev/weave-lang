#pragma once
#include <string_view>
#include <optional>

namespace weave::session
{
    struct SanitizersSpec final
    {
        bool AddressSanitizer{};
        bool ThreadSanitizer{};
        bool MemorySanitizer{};
        bool LeakSanitizer{};

        [[nodiscard]] static bool Parse(SanitizersSpec& result, std::optional<std::string_view> const& value);
    };
}
