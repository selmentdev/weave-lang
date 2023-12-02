#pragma once
#include <array>
#include <compare>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string_view>

#include "weave/Bitwise.hxx"

#include <fmt/format.h>

namespace weave::uuid
{
    struct Uuid
    {
        uint8_t Elements[16];

        [[nodiscard]] constexpr auto operator<=>(Uuid const& other) const = default;
    };

    enum class UuidStringFormat
    {
        None, // XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
        Dashes, // XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX
        Braces, // {XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX}
        BracesDashes, // {XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX}
    };

    [[nodiscard]] inline Uuid Make(uint64_t upper, uint64_t lower)
    {
        Uuid result;

        bitwise::StoreUnalignedLittleEndian(&result.Elements[0], lower);
        bitwise::StoreUnalignedLittleEndian(&result.Elements[8], upper);

        return result;
    }

    [[nodiscard]] Uuid FromBytes(std::span<std::byte const> bytes);

    [[nodiscard]] Uuid FromNamespace(Uuid const& ns, std::string_view name);

    [[nodiscard]] std::span<char> TryToChars(std::span<char> buffer, Uuid const& value, UuidStringFormat format);

    [[nodiscard]] std::optional<Uuid> TryFromChars(std::string_view value);
}

template <>
struct fmt::formatter<weave::uuid::Uuid>
{
private:
    weave::uuid::UuidStringFormat _format{weave::uuid::UuidStringFormat::BracesDashes};

public:
    constexpr auto parse(auto& context)
    {
        auto it = context.begin();

        if (it != context.end())
        {
            switch (*it)
            {
            case 'n':
                {
                    ++it;
                    this->_format = weave::uuid::UuidStringFormat::None;
                    break;
                }
            case 'd':
                {
                    ++it;
                    this->_format = weave::uuid::UuidStringFormat::Dashes;
                    break;
                }
            case 'b':
                {
                    ++it;
                    this->_format = weave::uuid::UuidStringFormat::Braces;
                    break;
                }

            case 'f':
                {
                    ++it;
                    this->_format = weave::uuid::UuidStringFormat::BracesDashes;
                    break;
                }

            default:
                {
                    break;
                }
            }
        }

        return it;
    }

    auto format(weave::uuid::Uuid const& value, auto& context)
    {
        std::array<char, 64> buffer;
        std::span<char> processed = weave::uuid::TryToChars(buffer, value, this->_format);
        return std::copy(processed.begin(), processed.end(), context.out());
    }
};
