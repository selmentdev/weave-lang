#pragma once
#include "weave/platform/windows/PlatformHeaders.hxx"

#include <memory>
#include <string_view>
#include <span>
#include <cassert>

namespace weave::platform::windows
{
    template <typename CharT, size_t StaticCapacityT>
    class win32_string_buffer final
    {
    private:
        CharT* m_Data{};
        std::unique_ptr<CharT[]> m_Dynamic{};
        size_t m_Capacity{StaticCapacityT};
        CharT m_Static[StaticCapacityT];

    public:
        constexpr win32_string_buffer() noexcept
            : m_Data{m_Static}
        {
        }

        constexpr explicit win32_string_buffer(std::basic_string_view<CharT> value)
        {
            this->resize_for_override(value.size() + 1);
            std::copy(value.begin(), value.end(), this->m_Data);
            this->trim(value.size());
        }

        win32_string_buffer(win32_string_buffer const&) = delete;
        win32_string_buffer(win32_string_buffer&&) = delete;
        win32_string_buffer& operator=(win32_string_buffer const&) = delete;
        win32_string_buffer& operator=(win32_string_buffer&&) = delete;

        ~win32_string_buffer() noexcept = default;

        constexpr void resize_for_override(size_t size)
        {
            if (size < StaticCapacityT)
            {
                // Reset back to static buffer.
                this->m_Dynamic = nullptr;
                this->m_Data = this->m_Static;
                this->m_Capacity = StaticCapacityT;
            }
            else if (size > this->m_Capacity)
            {
                size_t const capacity = size;

                this->m_Dynamic = std::make_unique_for_overwrite<CharT[]>(capacity);
                this->m_Data = this->m_Dynamic.get();
                this->m_Capacity = capacity;
            }
        }

        constexpr void trim(size_t size) noexcept
        {
            assert(size <= this->m_Capacity);
            this->m_Data[size] = {};
        }

        [[nodiscard]] constexpr size_t capacity() const noexcept
        {
            return this->m_Capacity;
        }

        [[nodiscard]] constexpr operator const CharT*() const noexcept
        {
            return this->m_Data;
        }

        [[nodiscard]] constexpr CharT* data() noexcept
        {
            return this->m_Data;
        }

        [[nodiscard]] constexpr const CharT* data() const noexcept
        {
            return this->m_Data;
        }

        [[nodiscard]] constexpr const CharT* c_str() const noexcept
        {
            return this->m_Data;
        }

        [[nodiscard]] constexpr std::basic_string_view<CharT> as_view() const noexcept
        {
            return std::basic_string_view<CharT>{this->m_Data};
        }

        [[nodiscard]] constexpr std::span<CharT> as_span() noexcept
        {
            return std::span<CharT>{this->m_Data, this->m_Capacity};
        }

        [[nodiscard]] constexpr std::span<CharT const> as_span() const noexcept
        {
            return std::span<CharT const>{this->m_Data, this->m_Capacity};
        }
    };

    template <typename StringBufferT, typename CallbackT>
    bool win32_adapt_string_buffer(StringBufferT& buffer, CallbackT&& callback) noexcept
    {
        size_t requiredCapacity{};
        if (not callback(buffer.as_span(), requiredCapacity))
        {
            buffer.trim(0);
            return false;
        }

        if (requiredCapacity <= buffer.capacity())
        {
            assert(requiredCapacity != 0);
            buffer.trim(requiredCapacity - 1);
            return true;
        }

        size_t bufferLength{};
        do
        {
            bufferLength = requiredCapacity;

            buffer.resize_for_override(bufferLength);

            if (not callback(buffer.as_span(), requiredCapacity))
            {
                buffer.trim(0);
                return false;
            }
        } while (requiredCapacity > bufferLength);

        assert(requiredCapacity != 0);
        buffer.trim(requiredCapacity - 1);
        return true;
    }

    using win32_FilePathW = win32_string_buffer<wchar_t, MAX_PATH>;
    using win32_FilePathA = win32_string_buffer<char, MAX_PATH>;

    bool win32_WidenString(std::wstring& result, std::string_view value) noexcept;

    template <size_t CapacityT>
    bool win32_WidenString(win32_string_buffer<wchar_t, CapacityT>& result, std::string_view value) noexcept
    {
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
                result.trim(0);
                return false;
            }

            result.resize_for_override(static_cast<size_t>(required) + 1);

            std::span<wchar_t> const writable = result.as_span();

            int const processed = MultiByteToWideChar(
                CP_UTF8,
                0,
                value.data(),
                length,
                writable.data(),
                static_cast<int>(writable.size()));

            if (processed == required)
            {
                result.trim(static_cast<size_t>(processed));
                return true;
            }

            return false;
        }

        result.trim(0);
        return true;
    }

    bool win32_NarrowString(std::string& result, std::wstring_view value) noexcept;

    template <size_t CapacityT>
    bool win32_NarrowString(win32_string_buffer<char, CapacityT>& result, std::wstring_view value) noexcept
    {
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
                result.trim(0);
                return false;
            }

            result.resize_for_override(static_cast<size_t>(required) + 1);

            std::span<char> const writable = result.as_span();

            int const processed = WideCharToMultiByte(
                CP_UTF8,
                0,
                value.data(),
                length,
                writable.data(),
                static_cast<int>(writable.size()),
                nullptr,
                nullptr);

            if (processed == required)
            {
                result.trim(static_cast<size_t>(processed));
                return true;
            }

            return false;
        }

        result.trim(0);
        return true;
    }
}
