#pragma once
#include "weave/platform/stringbuffer.hxx"

#if !defined(WIN32)
#error Available only for windows platform
#endif

namespace weave::platform
{
    bool widen_string_impl(
        void* context,
        void (*reserve)(void*, size_t),
        std::span<wchar_t> (*get)(void*),
        void (*trim)(void*, size_t),
        std::string_view value);

    bool narrow_string_impl(
        void* context,
        void (*reserve)(void*, size_t),
        std::span<char> (*get)(void*),
        void (*trim)(void*, size_t),
        std::wstring_view value);

    template <size_t CapacityT>
    bool widen_string(
        StringBuffer<wchar_t, CapacityT>& result,
        std::string_view value)
    {
        return widen_string_impl(
            &result,
            [](void* context, size_t const size)
            {
                static_cast<StringBuffer<wchar_t, CapacityT>*>(context)->resize_for_overwrite(size);
            },
            [](void* context) -> std::span<wchar_t>
            {
                return static_cast<StringBuffer<wchar_t, CapacityT>*>(context)->get_buffer_view();
            },
            [](void* context, size_t const size)
            {
                static_cast<StringBuffer<wchar_t, CapacityT>*>(context)->trim(size);
            },
            value);
    }

    template <size_t CapacityT>
    bool narrow_string(
        StringBuffer<char, CapacityT>& result,
        std::wstring_view value)
    {
        return NarrowStringImpl(
            &result,
            [](void* context, size_t const size)
            {
                static_cast<StringBuffer<char, CapacityT>*>(context)->resize_for_overwrite(size);
            },
            [](void* context) -> std::span<char>
            {
                return static_cast<StringBuffer<char, CapacityT>*>(context)->get_buffer_view();
            },
            [](void* context, size_t const size)
            {
                static_cast<StringBuffer<char, CapacityT>*>(context)->trim(size);
            },
            value);
    }
}
