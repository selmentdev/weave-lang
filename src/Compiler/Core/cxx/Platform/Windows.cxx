#include "Weave.Core/Platform/Windows.hxx"
#include "Weave.Core/Platform/Compiler.hxx"

#if defined(WIN32)

WEAVE_EXTERNAL_HEADERS_BEGIN

#define NOMINMAX
#include <Windows.h>

WEAVE_EXTERNAL_HEADERS_END

namespace Weave
{
    bool WidenStringImpl(
        void* context,
        void (*reserve)(void*, size_t),
        std::span<wchar_t> (*get)(void*),
        void (*trim)(void*, size_t),
        std::string_view value)
    {
        assert(value.length() < static_cast<size_t>(std::numeric_limits<int32_t>::max()));
        int const length = static_cast<int>(value.length());

        if (length > 0)
        {
            int const required = MultiByteToWideChar(
                CP_UTF8,
                0,
                value.data(),
                length,
                nullptr,
                0);

            if (required == 0)
            {
                trim(context, 0);
                return false;
            }

            reserve(context, static_cast<size_t>(required) + 1);
            std::span<wchar_t> const writable = get(context);

            int const processed = MultiByteToWideChar(
                CP_UTF8,
                0,
                value.data(),
                length,
                writable.data(),
                static_cast<int>(writable.size()));

            assert(processed == required);
            if (processed == required)
            {
                trim(context, static_cast<size_t>(processed));
                return true;
            }

            trim(context, 0);
            return false;
        }

        trim(context, 0);
        return true;
    }

    bool NarrowStringImpl(
        void* context,
        void (*reserve)(void*, size_t),
        std::span<char> (*get)(void*),
        void (*trim)(void*, size_t),
        std::wstring_view value)
    {
        assert(value.length() < static_cast<size_t>(std::numeric_limits<int32_t>::max()));
        int const length = static_cast<int>(value.length());

        if (length > 0)
        {
            int const required = WideCharToMultiByte(
                CP_UTF8,
                0,
                value.data(),
                length,
                nullptr,
                0,
                nullptr,
                nullptr);

            if (required == 0)
            {
                trim(context, 0);
                return false;
            }

            reserve(context, static_cast<size_t>(required) + 1);
            std::span<char> const writable = get(context);

            int const processed = WideCharToMultiByte(
                CP_UTF8,
                0,
                value.data(),
                length,
                writable.data(),
                static_cast<int>(writable.size()),
                nullptr,
                nullptr);

            assert(processed == required);
            if (processed == required)
            {
                trim(context, static_cast<size_t>(processed));
                return true;
            }

            trim(context, 0);
            return false;
        }

        trim(context, 0);
        return true;
    }
}

#endif
