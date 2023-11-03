#pragma once
#include "Weave.Core/Assert.hxx"
#include "Weave.Core/Source.hxx"
#include "Weave.Core/Memory/Allocator.hxx"
#include "Weave.Syntax/TokenKind.hxx"
#include "Weave.Syntax/TriviaKind.hxx"

// Design:
//
// - SyntaxContext - a place to hold state, allocations etc.
// -

namespace Weave::Syntax
{
    enum class TriviaMode
    {
        None,
        Documentation,
        All
    };

    struct Trivia final
    {
        TriviaKind Kind{};
        SourceSpan Source{};
    };

    struct Token final
    {
        TokenKind Kind{};

        SourceSpan Source{};

        std::span<Trivia const> LeadingTrivia{};
        std::span<Trivia const> TrailingTrivia{};

        void* Value{};
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

    struct IntegerLiteralValue final
    {
        NumberLiteralPrefix Prefix{};
        IntegerLiteralSuffix Suffix{};

        // FIXME: This should be a big integer type.
        std::string_view Value{};
    };

    struct FloatLiteralValue final
    {
        NumberLiteralPrefix Prefix{};
        FloatLiteralSuffix Suffix{};

        // FIXME: This should be a big float type.
        std::string_view Value{};
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

    struct StringLiteralValue final
    {
        StringPrefix Prefix{};
        std::string_view Value{};
    };

    struct CharacterLiteralValue final
    {
        CharacterPrefix Prefix{};
        char32_t Value;
    };

    struct Identifier final
    {
        std::string_view Value;
    };
}
