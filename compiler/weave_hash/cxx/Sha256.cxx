#include "weave/hash/Sha256.hxx"
#include "weave/Bitwise.hxx"

#include <cstring>

namespace weave::hash::sha256_impl
{
    static constexpr uint32_t K[64] = {
        0x428a2f98u, 0x71374491u, 0xb5c0fbcfu, 0xe9b5dba5u,
        0x3956c25bu, 0x59f111f1u, 0x923f82a4u, 0xab1c5ed5u,
        0xd807aa98u, 0x12835b01u, 0x243185beu, 0x550c7dc3u,
        0x72be5d74u, 0x80deb1feu, 0x9bdc06a7u, 0xc19bf174u,
        0xe49b69c1u, 0xefbe4786u, 0x0fc19dc6u, 0x240ca1ccu,
        0x2de92c6fu, 0x4a7484aau, 0x5cb0a9dcu, 0x76f988dau,
        0x983e5152u, 0xa831c66du, 0xb00327c8u, 0xbf597fc7u,
        0xc6e00bf3u, 0xd5a79147u, 0x06ca6351u, 0x14292967u,
        0x27b70a85u, 0x2e1b2138u, 0x4d2c6dfcu, 0x53380d13u,
        0x650a7354u, 0x766a0abbu, 0x81c2c92eu, 0x92722c85u,
        0xa2bfe8a1u, 0xa81a664bu, 0xc24b8b70u, 0xc76c51a3u,
        0xd192e819u, 0xd6990624u, 0xf40e3585u, 0x106aa070u,
        0x19a4c116u, 0x1e376c08u, 0x2748774cu, 0x34b0bcb5u,
        0x391c0cb3u, 0x4ed8aa4au, 0x5b9cca4fu, 0x682e6ff3u,
        0x748f82eeu, 0x78a5636fu, 0x84c87814u, 0x8cc70208u,
        0x90befffau, 0xa4506cebu, 0xbef9a3f7u, 0xc67178f2u};

    static constexpr std::byte Padding[64] = {std::byte{0x80}};

    constexpr uint32_t Ch(uint32_t x, uint32_t y, uint32_t z)
    {
        return ((x & y) ^ (~x & z));
    }

    constexpr uint32_t Maj(uint32_t x, uint32_t y, uint32_t z)
    {
        return ((x & y) ^ (x & z) ^ (y & z));
    }

    constexpr uint32_t Sigma1(uint32_t x)
    {
        return bitwise::RotateRight(x, 2) ^ bitwise::RotateRight(x, 13) ^ bitwise::RotateRight(x, 22);
    }

    constexpr uint32_t Sigma2(uint32_t x)
    {
        return bitwise::RotateRight(x, 6) ^ bitwise::RotateRight(x, 11) ^ bitwise::RotateRight(x, 25);
    }

    constexpr uint32_t Sigma3(uint32_t x)
    {
        return bitwise::RotateRight(x, 7) ^ bitwise::RotateRight(x, 18) ^ (x >> 3);
    }

    constexpr uint32_t Sigma4(uint32_t x)
    {
        return bitwise::RotateRight(x, 17) ^ bitwise::RotateRight(x, 19) ^ (x >> 10);
    }

    void Transform(Sha256& context)
    {
        std::array<uint32_t, 64> w;

        for (size_t i = 0; i < 16; ++i)
        {
            w[i] = bitwise::LoadUnalignedBigEndian<uint32_t>(&context.buffer[sizeof(uint32_t) * i]);
        }

        for (size_t i = 16; i < 64; ++i)
        {
            uint32_t const s0 = Sigma3(w[i - 15]);
            uint32_t const s1 = Sigma4(w[i - 2]);
            w[i] = w[i - 16] + s0 + w[i - 7] + s1;
        }

        uint32_t a = context.state[0];
        uint32_t b = context.state[1];
        uint32_t c = context.state[2];
        uint32_t d = context.state[3];
        uint32_t e = context.state[4];
        uint32_t f = context.state[5];
        uint32_t g = context.state[6];
        uint32_t h = context.state[7];

        for (size_t i = 0; i < 64; ++i)
        {
            uint32_t const s0 = Sigma1(a);
            uint32_t const maj = Maj(a, b, c);
            uint32_t const t2 = s0 + maj;

            uint32_t const s1 = Sigma2(e);
            uint32_t const ch = Ch(e, f, g);
            uint32_t const t1 = h + s1 + ch + K[i] + w[i];

            h = g;
            g = f;
            f = e;
            e = d + t1;
            d = c;
            c = b;
            b = a;
            a = t1 + t2;
        }

        context.state[0] += a;
        context.state[1] += b;
        context.state[2] += c;
        context.state[3] += d;
        context.state[4] += e;
        context.state[5] += f;
        context.state[6] += g;
        context.state[7] += h;
    }
}

namespace weave::hash
{
    void Sha256Initialize(Sha256& context)
    {
        context.state[0] = 0x6a09e667u;
        context.state[1] = 0xbb67ae85u;
        context.state[2] = 0x3c6ef372u;
        context.state[3] = 0xa54ff53au;
        context.state[4] = 0x510e527fu;
        context.state[5] = 0x9b05688cu;
        context.state[6] = 0x1f83d9abu;
        context.state[7] = 0x5be0cd19u;
        context.count = 0;
        context.size = 0;
    }

    void Sha256Update(Sha256& context, std::byte const* buffer, size_t length)
    {
        while (length > 0)
        {
            size_t const n = std::min(length, 64 - context.size);

            std::memcpy(&context.buffer[context.size], buffer, n);

            context.size += n;
            context.count += n;

            length -= n;
            buffer += n;

            if (context.size == 64)
            {
                sha256_impl::Transform(context);
                context.size = 0;
            }
        }
    }

    auto Sha256Finalize(Sha256& context) -> std::array<uint8_t, 32>
    {
        uint64_t const total_size = context.count * 8;

        size_t paddingSize;

        if (context.size < 56)
        {
            paddingSize = 56 - context.size;
        }
        else
        {
            paddingSize = 64 + 56 - context.size;
        }

        Sha256Update(context, sha256_impl::Padding, paddingSize);

        bitwise::StoreUnalignedBigEndian(&context.buffer[56], total_size);

        sha256_impl::Transform(context);

        std::array<uint8_t, 32> digest;

        for (size_t i = 0; i < 8; ++i)
        {
            bitwise::StoreUnalignedBigEndian(&digest[i * sizeof(uint32_t)], context.state[i]);
        }

        return digest;
    }

    auto Sha256FromBuffer(std::byte const* buffer, size_t length) -> std::array<uint8_t, 32>
    {
        Sha256 context;
        Sha256Initialize(context);
        Sha256Update(context, buffer, length);
        return Sha256Finalize(context);
    }

    auto Sha256FromString(std::string_view value) -> std::array<uint8_t, 32>
    {
        Sha256 context;
        Sha256Initialize(context);
        Sha256Update(context, reinterpret_cast<std::byte const*>(value.data()), value.size());
        return Sha256Finalize(context);
    }
}
