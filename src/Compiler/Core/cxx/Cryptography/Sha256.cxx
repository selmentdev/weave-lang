#include "Compiler.Core/Cryptography/Sha256.hxx"

namespace Weave::Cryptography::Sha256Private
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

    constexpr uint32_t RotateRight(uint32_t value, size_t count)
    {
        return (value >> count) | (value << (32 - count));
    }

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
        return RotateRight(x, 2) ^ RotateRight(x, 13) ^ RotateRight(x, 22);
    }

    constexpr uint32_t Sigma2(uint32_t x)
    {
        return RotateRight(x, 6) ^ RotateRight(x, 11) ^ RotateRight(x, 25);
    }

    constexpr uint32_t Sigma3(uint32_t x)
    {
        return RotateRight(x, 7) ^ RotateRight(x, 18) ^ (x >> 3);
    }

    constexpr uint32_t Sigma4(uint32_t x)
    {
        return RotateRight(x, 17) ^ RotateRight(x, 19) ^ (x >> 10);
    }

    void Transform(Sha256Context& context)
    {
        std::array<uint32_t, 64> w{};

        ToBigEndianBuffer(w.data(), context.Buffer.data(), 16);

        for (size_t i = 16; i < 64; ++i)
        {
            uint32_t const s0 = Sigma3(w[i - 15]);
            uint32_t const s1 = Sigma4(w[i - 2]);
            w[i] = w[i - 16] + s0 + w[i - 7] + s1;
        }

        uint32_t a = context.State[0];
        uint32_t b = context.State[1];
        uint32_t c = context.State[2];
        uint32_t d = context.State[3];
        uint32_t e = context.State[4];
        uint32_t f = context.State[5];
        uint32_t g = context.State[6];
        uint32_t h = context.State[7];

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

        context.State[0] += a;
        context.State[1] += b;
        context.State[2] += c;
        context.State[3] += d;
        context.State[4] += e;
        context.State[5] += f;
        context.State[6] += g;
        context.State[7] += h;
    }
}

namespace Weave::Cryptography
{
    void Sha256Initialize(Sha256Context& context)
    {
        context.State[0] = 0x6a09e667u;
        context.State[1] = 0xbb67ae85u;
        context.State[2] = 0x3c6ef372u;
        context.State[3] = 0xa54ff53au;
        context.State[4] = 0x510e527fu;
        context.State[5] = 0x9b05688cu;
        context.State[6] = 0x1f83d9abu;
        context.State[7] = 0x5be0cd19u;
        context.Count = 0;
        context.Size = 0;
    }

    void Sha256Update(Sha256Context& context, std::byte const* buffer, size_t length)
    {
        while (length > 0)
        {
            size_t n = std::min(length, 64 - context.Size);

            std::memcpy(&context.Buffer[context.Size], buffer, n);

            context.Size += n;
            context.Count += n;

            length -= n;

            if (context.Size == 64)
            {
                Sha256Private::Transform(context);
                context.Size = 0;
            }
        }
    }

    Sha256Digest Sha256Final(Sha256Context& context)
    {
        uint64_t const totalSize = context.Count * 8;

        size_t paddingSize;

        if (context.Size < 56)
        {
            paddingSize = 56 - context.Size;
        }
        else
        {
            paddingSize = 64 + 56 - context.Size;
        }

        Sha256Update(context, Sha256Private::Padding, paddingSize);

        ToBigEndianBuffer(context.Buffer.data() + 56, &totalSize, 1);

        Sha256Private::Transform(context);

        Sha256Digest digest{};

        ToBigEndianBuffer(digest.data(), context.State.data(), 8);

        return digest;
    }    
}
