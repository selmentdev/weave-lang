#pragma once
#include "Compiler.Core/StringBuffer.hxx"

#if defined(WIN32)

namespace Weave
{
    bool WidenStringImpl(
        void* context,
        void (*reserve)(void*, size_t),
        std::span<wchar_t> (*get)(void*),
        void (*trim)(void*, size_t),
        std::string_view value);

    bool NarrowStringImpl(
        void* context,
        void (*reserve)(void*, size_t),
        std::span<char> (*get)(void*),
        void (*trim)(void*, size_t),
        std::wstring_view value);

    template <size_t CapacityT>
    bool WidenString(
        StringBuffer<wchar_t, CapacityT>& result,
        std::string_view value)
    {
        return WidenStringImpl(
            &result,
            [](void* context, size_t const size)
            {
                static_cast<StringBuffer<wchar_t, CapacityT>*>(context)->ResizeForOverwrite(size);
            },
            [](void* context) -> std::span<wchar_t>
            {
                return static_cast<StringBuffer<wchar_t, CapacityT>*>(context)->GetBufferView();
            },
            [](void* context, size_t const size)
            {
                static_cast<StringBuffer<wchar_t, CapacityT>*>(context)->Trim(size);
            },
            value);
    }

    template <size_t CapacityT>
    bool NarrowString(
        StringBuffer<char, CapacityT>& result,
        std::wstring_view value)
    {
        return NarrowStringImpl(
            &result,
            [](void* context, size_t const size)
            {
                static_cast<StringBuffer<char, CapacityT>*>(context)->ResizeForOverwrite(size);
            },
            [](void* context) -> std::span<char>
            {
                return static_cast<StringBuffer<char, CapacityT>*>(context)->GetBufferView();
            },
            [](void* context, size_t const size)
            {
                static_cast<StringBuffer<char, CapacityT>*>(context)->Trim(size);
            },
            value);
    }
}

#endif
