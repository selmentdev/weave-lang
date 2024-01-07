#pragma once
#include <type_traits>

namespace weave::bitwise
{
    template <typename EnumT>
        requires(std::is_enum_v<EnumT>)
    struct Flags
    {
    public:
        using UnderlyingType = std::underlying_type_t<EnumT>;
        using ValueType = EnumT;

    private:
        ValueType _value{};

    public:
        constexpr Flags() = default;

        constexpr Flags(ValueType value)
            : _value{value}
        {
        }

        constexpr Flags(UnderlyingType value)

            : _value{static_cast<ValueType>(value)}
        {
        }

        [[nodiscard]] constexpr operator ValueType() const
        {
            return static_cast<ValueType>(this->_value);
        }

        [[nodiscard]] explicit constexpr operator UnderlyingType() const
        {
            return this->_value;
        }

        [[nodiscard]] constexpr Flags operator|(Flags other) const
        {
            return Flags{static_cast<ValueType>(std::to_underlying(this->_value) | std::to_underlying(other._value))};
        }

        [[nodiscard]] constexpr Flags operator&(Flags other) const
        {
            return Flags{static_cast<ValueType>(std::to_underlying(this->_value) & std::to_underlying(other._value))};
        }

        [[nodiscard]] constexpr Flags operator^(Flags other) const
        {
            return Flags{static_cast<ValueType>(std::to_underlying(this->_value) ^ std::to_underlying(other._value))};
        }

        [[nodiscard]] constexpr Flags operator~() const
        {
            return Flags{static_cast<ValueType>(~std::to_underlying(this->_value))};
        }

        constexpr Flags& operator|=(Flags other)
        {
            this->_value = static_cast<ValueType>(std::to_underlying(this->_value) | std::to_underlying(other._value));
            return *this;
        }

        constexpr Flags& operator&=(Flags other)
        {
            this->_value = static_cast<ValueType>(std::to_underlying(this->_value) & std::to_underlying(other._value));
            return *this;
        }

        constexpr Flags& operator^=(Flags other)
        {
            this->_value = static_cast<ValueType>(std::to_underlying(this->_value) ^ std::to_underlying(other._value));
            return *this;
        }

        [[nodiscard]] constexpr bool operator!() const
        {
            return this->_value == ValueType{};
        }

        [[nodiscard]] constexpr auto operator<=>(Flags const& other) const = default;


        [[nodiscard]] constexpr bool All(Flags other) const
        {
            return static_cast<ValueType>(std::to_underlying(this->_value) & std::to_underlying(other._value)) == other._value;
        }

        [[nodiscard]] constexpr bool Any(Flags other) const
        {
            return static_cast<ValueType>(std::to_underlying(this->_value) & std::to_underlying(other._value)) != ValueType{};
        }

        [[nodiscard]] constexpr bool None(Flags other) const
        {
            return static_cast<ValueType>(std::to_underlying(this->_value) & std::to_underlying(other._value)) == ValueType{};
        }
    };
}
