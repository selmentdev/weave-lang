#pragma once
#include "weave/BugCheck.hxx"
#include "weave/source/Source.hxx"
#include "weave/syntax/TokenKind.hxx"
#include "weave/syntax/TriviaKind.hxx"

#include <span>

// Design:
//
// - SyntaxContext - a place to hold state, allocations etc.
// -

namespace weave::syntax
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
        source::SourceSpan Source{};
    };

    enum class NumberLiteralPrefixKind
    {
        Default,
        Binary,
        Octal,
        Decimal,
        Hexadecimal,
    };

    enum class IntegerLiteralSuffixKind
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

    enum class FloatLiteralSuffixKind
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
        NumberLiteralPrefixKind Prefix{};
        IntegerLiteralSuffixKind Suffix{};

        // FIXME: This should be a big integer type.
        std::string_view Value{};
    };

    struct FloatLiteralValue final
    {
        NumberLiteralPrefixKind Prefix{};
        FloatLiteralSuffixKind Suffix{};

        // FIXME: This should be a big float type.
        std::string_view Value{};
    };

    enum class StringPrefixKind
    {
        Default, // ""
        Utf8, // u8""
        Utf16, // u""
        Utf32, // U""
    };

    enum class CharacterPrefixKind
    {
        Default, // ''
        Utf8, // u8''
        Utf16, // u''
        Utf32, // U''
    };

    struct StringLiteralValue final
    {
        StringPrefixKind Prefix{};
        std::string_view Value{};
    };

    struct CharacterLiteralValue final
    {
        CharacterPrefixKind Prefix{};
        char32_t Value;
    };

    struct Identifier final
    {
        std::string_view Value;
    };

    struct Token final
    {
    private:
        TokenKind _kind{};
        source::SourceSpan _source{};
        uint16_t _leading_trivia_count{};
        uint16_t _trailing_trivia_count{};
        Trivia const* _trivia{};
        void* _value{};

    public:
        Token(TokenKind kind, source::SourceSpan source)
            : _kind{kind}
            , _source{source}
        {
        }

        Token(TokenKind kind, source::SourceSpan source, Trivia const* trivia, uint16_t leading_trivia_count, uint16_t trailing_trivia_count)
            : _kind{kind}
            , _source{source}
            , _leading_trivia_count{leading_trivia_count}
            , _trailing_trivia_count{trailing_trivia_count}
            , _trivia{trivia}
        {
        }

        Token(TokenKind kind, source::SourceSpan source, Trivia const* trivia, uint16_t leading_trivia_count, uint16_t trailing_trivia_count, void* value)
            : _kind{kind}
            , _source{source}
            , _leading_trivia_count{leading_trivia_count}
            , _trailing_trivia_count{trailing_trivia_count}
            , _trivia{trivia}
            , _value{value}
        {
        }

        TokenKind GetKind() const
        {
            return this->_kind;
        }

        source::SourceSpan const& GetSource() const
        {
            return this->_source;
        }

        std::span<Trivia const> GetLeadingTrivia() const
        {
            return std::span{this->_trivia, this->_leading_trivia_count};
        }

        std::span<Trivia const> GetTrailingTrivia() const
        {
            return std::span{this->_trivia + this->_leading_trivia_count, this->_trailing_trivia_count};
        }

        constexpr IntegerLiteralValue const* TryGetIntegerValue() const
        {
            if (this->_kind == TokenKind::IntegerLiteral)
            {
                return static_cast<IntegerLiteralValue const*>(this->_value);
            }

            return nullptr;
        }

        constexpr FloatLiteralValue const* TryGetFloatValue() const
        {
            if (this->_kind == TokenKind::FloatLiteral)
            {
                return static_cast<FloatLiteralValue const*>(this->_value);
            }

            return nullptr;
        }

        constexpr StringLiteralValue const* TryGetStringValue() const
        {
            if (this->_kind == TokenKind::StringLiteral)
            {
                return static_cast<StringLiteralValue const*>(this->_value);
            }

            return nullptr;
        }

        constexpr CharacterLiteralValue const* TryGetCharacterValue() const
        {
            if (this->_kind == TokenKind::CharacterLiteral)
            {
                return static_cast<CharacterLiteralValue const*>(this->_value);
            }

            return nullptr;
        }

        IntegerLiteralValue const& GetIntegerValue() const
        {
            WEAVE_ASSERT(this->_kind == TokenKind::IntegerLiteral);
            return *static_cast<IntegerLiteralValue const*>(this->_value);
        }

        FloatLiteralValue const& GetFloatValue() const
        {
            WEAVE_ASSERT(this->_kind == TokenKind::FloatLiteral);
            return *static_cast<FloatLiteralValue const*>(this->_value);
        }

        StringLiteralValue const& GetStringValue() const
        {
            WEAVE_ASSERT(this->_kind == TokenKind::StringLiteral);
            return *static_cast<StringLiteralValue const*>(this->_value);
        }

        CharacterLiteralValue const& GetCharacterValue() const
        {
            WEAVE_ASSERT(this->_kind == TokenKind::CharacterLiteral);
            return *static_cast<CharacterLiteralValue const*>(this->_value);
        }
    };
}
