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
            this->resize_for_overwrite(value.size() + 1);
            std::copy(value.begin(), value.end(), this->_data);
            this->trim(value.size());
        }

    public:
        StringBuffer(StringBuffer const&) = delete;
        StringBuffer(StringBuffer&&) = delete;
        StringBuffer& operator=(StringBuffer const&) = delete;
        StringBuffer& operator=(StringBuffer&&) = delete;

        ~StringBuffer() = default;

        constexpr void resize_for_overwrite(size_t size)
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

        constexpr void trim(size_t size)
        {
            assert(size <= this->_capacity);
            this->_data[size] = {};
        }

        [[nodiscard]] constexpr std::basic_string_view<CharT> as_view() const
        {
            return std::basic_string_view<CharT>{this->_data};
        }

        [[nodiscard]] constexpr std::span<CharT> get_buffer_view()
        {
            return std::span<CharT>{this->_data, this->_capacity};
        }

        [[nodiscard]] constexpr std::span<CharT const> get_buffer_view() const
        {
            return std::span<CharT const>{this->_data, this->_capacity};
        }

        [[nodiscard]] CharT const* get_buffer() const
        {
            return this->_data;
        }

        [[nodiscard]] CharT* get_buffer()
        {
            return this->_data;
        }
    };
}
