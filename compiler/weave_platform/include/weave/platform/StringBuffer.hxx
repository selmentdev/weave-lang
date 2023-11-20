#pragma once
#include <memory>
#include <string_view>
#include <span>
#include <cassert>

namespace weave::platform
{
    template <typename CharT, size_t StaticCapacityT>
    class StringBuffer final
    {
    private:
        CharT* _data{};
        std::unique_ptr<CharT[]> _dynamic{};
        size_t _capacity{StaticCapacityT};
        CharT _static[StaticCapacityT];

    public:
        constexpr StringBuffer()
            : _data{_static}
        {
        }

        constexpr explicit StringBuffer(std::basic_string_view<CharT> value)
        {
            this->ResizeForOverwrite(value.size() + 1);
            std::copy(value.begin(), value.end(), this->_data);
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
                this->_dynamic = nullptr;
                this->_data = this->_static;
                this->_capacity = StaticCapacityT;
            }
            else if (size > this->_capacity)
            {
                size_t const capacity = size;

                this->_dynamic = std::make_unique_for_overwrite<CharT[]>(capacity);
                this->_data = this->_dynamic.get();
                this->_capacity = capacity;
            }
        }

        constexpr void Trim(size_t size)
        {
            assert(size <= this->_capacity);
            this->_data[size] = {};
        }

        [[nodiscard]] constexpr std::basic_string_view<CharT> AsView() const
        {
            return std::basic_string_view<CharT>{this->_data};
        }

        [[nodiscard]] constexpr std::span<CharT> GetBufferView()
        {
            return std::span<CharT>{this->_data, this->_capacity};
        }

        [[nodiscard]] constexpr std::span<CharT const> GetBufferView() const
        {
            return std::span<CharT const>{this->_data, this->_capacity};
        }

        [[nodiscard]] CharT const* GetBuffer() const
        {
            return this->_data;
        }

        [[nodiscard]] CharT* GetBuffer()
        {
            return this->_data;
        }
    };
}
