#pragma once
#include "weave/platform/Compiler.hxx"

#if !defined(WIN32)
#error "This header requires Windows Platform"
#else
WEAVE_EXTERNAL_HEADERS_BEGIN

#define NOMINMAX
#include <Windows.h>

WEAVE_EXTERNAL_HEADERS_END
#endif

namespace weave::platform::windows
{
    inline constexpr int64_t NanosecondsInSecond = 1'000'000'000;
    inline constexpr int64_t DateTimeEpochAdjust = 504911231999999999;

    [[nodiscard]] constexpr int64_t IntoTicks(FILETIME const value)
    {
        return std::bit_cast<int64_t>(value) + DateTimeEpochAdjust;
    }
}
