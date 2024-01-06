#pragma once
#include <type_traits>

namespace weave::bitwise
{
    template <typename EnumT>
        requires(std::is_enum_v<EnumT>)
    struct Flags
    {
        using Type = std::underlying_type_t<EnumT>;

        Type m_value;

        constexpr Flags() noexcept = default;

        constexpr Flags(EnumT value) noexcept

            : m_value(static_cast<Type>(value))
        {
        }

        constexpr Flags(Type value) noexcept

            : m_value(value)
        {
        }

        [[nodiscard]] constexpr operator EnumT() const noexcept
        {
            return static_cast<EnumT>(m_value);
        }

        [[nodiscard]] constexpr operator Type() const noexcept
        {
            return m_value;
        }

        [[nodiscard]] constexpr Flags operator|(Flags other) const noexcept
        {
            return Flags(m_value | other.m_value);
        }

        [[nodiscard]] constexpr Flags operator&(Flags other) const noexcept
        {
            return Flags(m_value & other.m_value);
        }

        [[nodiscard]] constexpr Flags operator^(Flags other) const noexcept
        {
            return Flags(m_value ^ other.m_value);
        }

        [[nodiscard]] constexpr Flags operator~() const noexcept
        {
            return Flags(~m_value);
        }

        constexpr Flags& operator|=(Flags other) noexcept
        {
            m_value |= other.m_value;
            return *this;
        }

        constexpr Flags& operator&=(Flags other) noexcept
        {
            m_value &= other.m_value;
            return *this;
        }

        constexpr Flags& operator^=(Flags other) noexcept
        {
            m_value ^= other.m_value;
            return *this;
        }

        [[nodiscard]] constexpr bool operator!() const noexcept
        {
            return !m_value;
        }

        [[nodiscard]] constexpr auto operator<=>(Flags const& other) const = default;


        [[nodiscard]] constexpr bool All(Flags other) const noexcept
        {
            return (m_value & other.m_value) == other.m_value;
        }

        [[nodiscard]] constexpr bool Any(Flags other) const noexcept
        {
            return (m_value & other.m_value) != 0;
        }

        [[nodiscard]] constexpr bool None(Flags other) const noexcept
        {
            return (m_value & other.m_value) == 0;
        }


    };
}
