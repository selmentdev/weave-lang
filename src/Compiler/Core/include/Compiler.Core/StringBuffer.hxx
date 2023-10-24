#pragma once
#include <memory>
#include <string_view>
#include <span>
#include <cassert>

namespace Weave
{
    template <typename CharT, size_t StaticCapacityT>
    class StringBuffer final
    {
    private:
        CharT* m_Data{};
        std::unique_ptr<CharT[]> m_Dynamic{};
        size_t m_Capacity{StaticCapacityT};
        CharT m_Static[StaticCapacityT];

    public:
        constexpr StringBuffer()
            : m_Data{m_Static}
        {
        }

        constexpr explicit StringBuffer(std::basic_string_view<CharT> value)
        {
            this->ResizeForOverwrite(value.size() + 1);
            std::copy(value.begin(), value.end(), this->m_Data);
            this->Trim(value.size());
        }

    public:
        StringBuffer(StringBuffer const&) = delete;
        StringBuffer(StringBuffer&&) = delete;
        StringBuffer& operator=(StringBuffer const&) = delete;
        StringBuffer& operator=(StringBuffer&&) = delete;

        ~StringBuffer() = default;

        constexpr void ResizeForOverwrite(size_t size)
        {
            if (size < StaticCapacityT)
            {
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

        constexpr void Trim(size_t size)
        {
            assert(size <= this->m_Capacity);
            this->m_Data[size] = {};
        }

        [[nodiscard]] constexpr std::basic_string_view<CharT> AsView() const
        {
            return std::basic_string_view<CharT>{this->m_Data};
        }

        [[nodiscard]] constexpr std::span<CharT> GetBufferView()
        {
            return std::span<CharT>{this->m_Data, this->m_Capacity};
        }

        [[nodiscard]] constexpr std::span<CharT const> GetBufferView() const
        {
            return std::span<CharT const>{this->m_Data, this->m_Capacity};
        }

        [[nodiscard]] CharT const* GetBuffer() const
        {
            return this->m_Data;
        }
    };
}
