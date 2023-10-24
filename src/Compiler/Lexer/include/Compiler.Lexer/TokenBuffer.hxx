#pragma once
#include "Compiler.Core/Source.hxx"
#include "Compiler.Lexer/TokenKind.hxx"
#include "Compiler.Lexer/TriviaKind.hxx"

namespace Weave::Lexer
{
    struct TokenTrivia final
    {
        TriviaKind Kind{};
        SourceSpan Source{};
    };

    struct Token final
    {
        TokenKind Kind{};
        SourceSpan Source{};
    };

    enum class IntegerLiteralKind
    {
        Default,
        Int8,
        Int16,
        Int32,
        Int64,
        Int128,
        UInt8,
        UInt16,
        UInt32,
        UInt64,
        UInt128,

        ISize,
        USize,

        IntPtr,
        UIntPtr,
    };

    enum class NumberRadix
    {
        Default,

        Binary,
        Octal,
        Decimal,
        Hexadecimal,
    };

    struct IntegerLiteralToken final
    {
        IntegerLiteralKind Kind{};

        NumberRadix Radix{};

        std::string Value{};
    };

    enum class RealLiteralKind
    {
        Default,

        Float16,
        Float32,
        Float64,
        Float128,

        Decimal128,
    };

    struct RealLiteralToken final
    {
        RealLiteralKind Kind{};

        NumberRadix Radix{};

        std::string Value{};
    };

    enum class TextEncoding
    {
        Default,
        Utf8,
        Utf16,
        Utf32,
    };

    struct StringToken final
    {
        std::string Value{};
    };

    struct CharacterToken final
    {
        std::string Value{};
    };

    struct IdentifierToken final
    {
        std::string Value{};
    };

    class TokenBufferView
    {
    };
}
