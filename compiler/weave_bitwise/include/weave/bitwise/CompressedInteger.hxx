#pragma once
#include <cstdint>

namespace weave::bitwise
{
    size_t DecodeUnsigned(uint64_t& result, uint8_t const* first, uint8_t const* last);
    size_t EncodeUnsigned(uint64_t value, uint8_t* first, uint8_t* last);
    size_t DecodeSigned(int64_t& result, uint8_t const* first, uint8_t const* last);
    size_t EncodeSigned(int64_t value, uint8_t* first, uint8_t* last);
}
