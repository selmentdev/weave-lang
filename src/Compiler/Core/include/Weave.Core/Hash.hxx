#pragma once
#include <cstdint>
#include <string_view>
#include <span>

namespace Weave
{
    struct Fnv1a64 final
    {
    private:
        static constexpr uint64_t InitialValue{0xcbf29ce484222325u};
        static constexpr uint64_t Multiplier{0x100000001B3u};

    private:
        uint64_t m_State{InitialValue};

    public:
        constexpr Fnv1a64() = default;

        constexpr Fnv1a64(uint64_t state)
            : m_State{state}
        {
        }

        constexpr Fnv1a64& Update(std::string_view value)
        {
            for (char const c : value)
            {
                this->m_State ^= static_cast<unsigned char>(c);
                this->m_State *= Multiplier;
            }

            return *this;
        }

        constexpr Fnv1a64& Update(std::span<unsigned char const> value)
        {
            for (unsigned char const c : value)
            {
                this->m_State ^= c;
                this->m_State *= Multiplier;
            }

            return *this;
        }

        [[nodiscard]] constexpr uint64_t Finish() const
        {
            return this->m_State;
        }

        [[nodiscard]] static constexpr uint64_t FromString(std::string_view value)
        {
            return Fnv1a64{}.Update(value).Finish();
        }
    };

    struct Fnv1a32 final
    {
    private:
        static constexpr uint32_t InitialValue{0x811c9dc5u};
        static constexpr uint32_t Multiplier{0x1000193u};

    private:
        uint32_t m_State{InitialValue};

    public:
        constexpr Fnv1a32() = default;

        constexpr Fnv1a32(uint32_t state)
            : m_State{state}
        {
        }

        constexpr Fnv1a32& Update(std::string_view value)
        {
            for (char const c : value)
            {
                this->m_State ^= static_cast<unsigned char>(c);
                this->m_State *= Multiplier;
            }

            return *this;
        }

        constexpr Fnv1a32& Update(std::span<unsigned char const> value)
        {
            for (unsigned char const c : value)
            {
                this->m_State ^= c;
                this->m_State *= Multiplier;
            }

            return *this;
        }

        [[nodiscard]] constexpr uint32_t Finish() const
        {
            return this->m_State;
        }

        [[nodiscard]] static constexpr uint32_t FromString(std::string_view value)
        {
            return Fnv1a32{}.Update(value).Finish();
        }
    };
}
