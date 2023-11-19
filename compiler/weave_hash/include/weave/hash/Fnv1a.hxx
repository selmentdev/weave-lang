#pragma once
#include <cstdint>
#include <string_view>
#include <span>

namespace weave::hash
{
    struct Fnv1a64 final
    {
    private:
        static constexpr uint64_t InitialValue{0xcbf29ce484222325u};
        static constexpr uint64_t Multiplier{0x100000001B3u};

    private:
        uint64_t _state{InitialValue};

    public:
        constexpr Fnv1a64() = default;

        constexpr Fnv1a64(uint64_t state)
            : _state{state}
        {
        }

        constexpr Fnv1a64& Update(std::string_view value)
        {
            for (char const c : value)
            {
                this->_state ^= static_cast<unsigned char>(c);
                this->_state *= Multiplier;
            }

            return *this;
        }

        constexpr Fnv1a64& Update(std::span<std::byte const> value)
        {
            for (std::byte const c : value)
            {
                this->_state ^= static_cast<unsigned char>(c);
                this->_state *= Multiplier;
            }

            return *this;
        }

        [[nodiscard]] constexpr uint64_t Finalize() const
        {
            return this->_state;
        }

        [[nodiscard]] static constexpr uint64_t FromString(std::string_view value)
        {
            return Fnv1a64{}.Update(value).Finalize();
        }

        [[nodiscard]] static constexpr uint64_t FromBuffer(std::span<std::byte const> value)
        {
            return Fnv1a64{}.Update(value).Finalize();
        }
    };
}

namespace weave::hash
{
    struct Fnv1a32 final
    {
    private:
        static constexpr uint32_t InitialValue{0x811c9dc5u};
        static constexpr uint32_t Multiplier{0x1000193u};

    private:
        uint32_t _state{InitialValue};

    public:
        constexpr Fnv1a32() = default;

        constexpr Fnv1a32(uint32_t state)
            : _state{state}
        {
        }

        constexpr Fnv1a32& Update(std::string_view value)
        {
            for (char const c : value)
            {
                this->_state ^= static_cast<unsigned char>(c);
                this->_state *= Multiplier;
            }

            return *this;
        }

        constexpr Fnv1a32& Update(std::span<std::byte const> value)
        {
            for (std::byte const c : value)
            {
                this->_state ^= static_cast<unsigned char>(c);
                this->_state *= Multiplier;
            }

            return *this;
        }

        [[nodiscard]] constexpr uint32_t Finalize() const
        {
            return this->_state;
        }

        [[nodiscard]] static constexpr uint32_t FromString(std::string_view value)
        {
            return Fnv1a32{}.Update(value).Finalize();
        }

        [[nodiscard]] static constexpr uint32_t FromBuffer(std::span<std::byte const> value)
        {
            return Fnv1a32{}.Update(value).Finalize();
        }
    };
}
