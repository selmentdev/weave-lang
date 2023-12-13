#include "weave/Uuid.hxx"
#include "weave/hash/Sha256.hxx"

#include <utility>

namespace weave::uuid::impl
{
    [[nodiscard]] constexpr char ToDigit(size_t value)
    {
        if (value < 10)
        {
            return static_cast<char>('0' + value);
        }

        if (value < 16)
        {
            return static_cast<char>('a' + (value - 10));
        }

        std::unreachable();
    }

    [[nodiscard]] constexpr int FromDigit(char value)
    {
        if ((value >= '0') and (value <= '9'))
        {
            return static_cast<unsigned char>(value - '0');
        }

        if ((value >= 'a') and (value <= 'f'))
        {
            return static_cast<unsigned char>(value - 'a') + 10;
        }

        if ((value >= 'A') and (value <= 'F'))
        {
            return static_cast<unsigned char>(value - 'A') + 10;
        }

        return -1;
    }
}

namespace weave::uuid
{
    Uuid FromBytes(std::span<std::byte const> bytes)
    {
        Uuid result{};

        size_t const count = std::min<size_t>(bytes.size(), 16);

        for (size_t i = 0; i < count; ++i)
        {
            result.Elements[i] = static_cast<uint8_t>(bytes[i]);
        }

        return result;
    }

    Uuid FromNamespace(Uuid const& ns, std::string_view name)
    {
        hash::Sha256 context;
        hash::Sha256Initialize(context);

        hash::Sha256Update(context, std::as_bytes(std::span{ns.Elements}));

        hash::Sha256Update(context, std::as_bytes(std::span{name}));

        auto const checksum = hash::Sha256Finalize(context);

        return FromBytes(std::as_bytes(std::span{checksum}));
    }


    std::span<char> TryToChars(std::span<char> buffer, Uuid const& value, UuidStringFormat format)
    {
        bool const braces = (format == UuidStringFormat::Braces) or (format == UuidStringFormat::BracesDashes);
        bool const dashes = (format == UuidStringFormat::Dashes) or (format == UuidStringFormat::BracesDashes);

        size_t required = 32;

        if (braces)
        {
            required += 2;
        }

        if (dashes)
        {
            required += 4;
        }

        if (buffer.size() >= required)
        {
            auto out = buffer.begin();

            if (braces)
            {
                (*out++) = '{';
            }

            size_t i = 0;
            for (uint8_t const octet : value.Elements)
            {
                size_t const lower = octet & 0x0Fu;
                size_t const upper = (octet >> 4) & 0x0Fu;

                if (dashes && (i == 4 || i == 6 || i == 8 || i == 10))
                {
                    (*out++) = '-';
                }

                (*out++) = impl::ToDigit(upper);
                (*out++) = impl::ToDigit(lower);

                ++i;
            }

            if (braces)
            {
                (*out) = '}';
            }

            return buffer.subspan(0, required);
        }

        return {};
    }

    std::optional<Uuid> TryFromChars(std::string_view value)
    {
        if (value.size() < 32)
        {
            return {};
        }

        if (value.front() == '{')
        {
            if (value.back() != '}')
            {
                return {};
            }

            value.remove_prefix(1);
            value.remove_suffix(1);
        }

        bool has_dashes = false;

        size_t i = 0;

        auto it = value.begin();
        auto const end = value.end();

        Uuid result;

        for (uint8_t& octet : result.Elements)
        {
            if (i == 4)
            {
                if (it != end)
                {
                    if (*it == '-')
                    {
                        has_dashes = true;

                        ++it;
                    }
                }
                else
                {
                    return {};
                }
            }
            else if (has_dashes && (i == 6 || i == 8 || i == 10))
            {
                if (it != end)
                {
                    if (*it == '-')
                    {
                        ++it;
                    }
                    else
                    {
                        return {};
                    }
                }
            }

            if (it == end)
            {
                return {};
            }

            int const upper = impl::FromDigit(*it);

            if (upper == -1)
            {
                return {};
            }

            ++it;

            if (it == end)
            {
                return {};
            }

            int const lower = impl::FromDigit(*it);

            if (lower == -1)
            {
                return {};
            }

            ++it;

            octet = static_cast<uint8_t>(upper);
            octet <<= 4;
            octet |= static_cast<uint8_t>(lower);

            ++i;
        }

        return result;
    }

}
