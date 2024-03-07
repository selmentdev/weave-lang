#pragma once
#include <cstdint>
#include <span>

namespace weave::hash
{
    struct AESContext
    {
        size_t Rounds{};
        uint32_t EncryptionKey[60];
        uint32_t DecryptionKey[60];
    };

    bool Initialize(AESContext& self, const uint8_t* key, size_t length);

    void Encrypt(AESContext const& self, const uint8_t* input, uint8_t* output);

    void Decrypt(AESContext const& self, const uint8_t* input, uint8_t* output);
}
