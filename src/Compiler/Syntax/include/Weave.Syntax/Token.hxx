#pragma once
#include "Weave.Core/Assert.hxx"
#include "Weave.Core/Source.hxx"
#include "Weave.Syntax/TokenKind.hxx"
#include "Weave.Core/Memory/VirtualHeap.hxx"

// Design:
//
// - SyntaxContext - a place to hold state, allocations etc.
// -

namespace Weave::Syntax
{
    struct LexerContext
    {
        Memory::VirtualHeap Heap{};

        Memory::VirtualHeap& GetHeap() noexcept
        {
            return this->Heap;
        }
    };
}

namespace Weave::Syntax
{
    struct Token
    {
        TokenKind Kind{};
        SourceSpan Source{};
    };

    enum class NumberLiteralPrefix
    {
        Default,
        Binary,
        Octal,
        Decimal,
        Hexadecimal,
    };

    enum class IntegerLiteralSuffix
    {
        Default, // No suffix

        Int8, // i8
        Int16, // i16
        Int32, // i32
        Int64, // i64
        Int128, // i128

        UInt8, // u8
        UInt16, // u16
        UInt32, // u32
        UInt64, // u64
        UInt128, // u128

        // These types can represent the size of a memory block.
        ISize, // isize
        USize, // usize

        // These types can represent an addresss of a memory block.
        // Note: on most platforms, these are the same size as ISize and USize. However, there are platforms with additional metadata.
        IntPtr, // iptr
        UIntPtr, // uptr
    };

    enum class FloatLiteralSuffix
    {
        Default,

        Float16,
        Float32,
        Float64,
        Float128,

        Decimal128,
    };

    struct IntegerLiteralToken final : Token
    {
        NumberLiteralPrefix Prefix{};
        IntegerLiteralSuffix Suffix{};

        // FIXME: This should be a big integer type.
        std::string Value{};
    };

    struct FloatLiteralToken final : Token
    {
        NumberLiteralPrefix Prefix{};
        FloatLiteralSuffix Type{};

        // FIXME: This should be a big float type.
        std::string Value{};
    };

    enum class StringPrefix
    {
        Default, // ""
        Utf8, // u8""
        Utf16, // u""
        Utf32, // U""
    };

    enum class CharacterPrefix
    {
        Default, // ''
        Utf8, // u8''
        Utf16, // u''
        Utf32, // U''
    };

    struct StringLiteralToken final : Token
    {
        StringPrefix Prefix{};
        std::string Value{};
    };

    struct CharacterLiteralToken final : Token
    {
        StringPrefix Prefix{};
        char32_t Value;

        static CharacterLiteralToken* Create(LexerContext& context, SourceSpan source, StringPrefix prefix, char32_t value);
    };
    static_assert(std::is_trivially_destructible_v<CharacterLiteralToken>);

    inline CharacterLiteralToken* CharacterLiteralToken::Create(LexerContext& context, SourceSpan source, StringPrefix prefix, char32_t value)
    {
        CharacterLiteralToken* result = static_cast<CharacterLiteralToken*>(context.GetHeap().Allocate(sizeof(CharacterLiteralToken), alignof(CharacterLiteralToken)));

        return new (result) CharacterLiteralToken{
            {
                .Kind = TokenKind::CharacterLiteral,
                .Source = source,
            },
            prefix,
            value,
        };
    }


}

namespace Weave::Syntax
{

}
