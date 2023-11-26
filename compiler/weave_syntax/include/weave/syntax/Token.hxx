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

    struct TriviaRange final
    {
        std::span<Trivia const> Leading{};
        std::span<Trivia const> Trailing{};
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
        char32_t Value{};
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
        TriviaRange const* _trivia{};
        void const* _data{};

    public:
        Token(TokenKind kind, source::SourceSpan const& source)
            : _kind{kind}
            , _source{source}
        {
        }

        Token(TokenKind kind, source::SourceSpan const& source, TriviaRange const* trivia)
            : _kind{kind}
            , _source{source}
            , _trivia{trivia}
        {
        }

        Token(source::SourceSpan const& source, TriviaRange const* trivia, IntegerLiteralValue const* value)
            : _kind{TokenKind::IntegerLiteral}
            , _source{source}
            , _trivia{trivia}
            , _data{value}
        {
        }

        Token(source::SourceSpan const& source, TriviaRange const* trivia, FloatLiteralValue const* value)
            : _kind{TokenKind::FloatLiteral}
            , _source{source}
            , _trivia{trivia}
            , _data{value}
        {
        }

        Token(source::SourceSpan const& source, TriviaRange const* trivia, StringLiteralValue const* value)
            : _kind{TokenKind::StringLiteral}
            , _source{source}
            , _trivia{trivia}
            , _data{value}
        {
        }

        Token(source::SourceSpan const& source, TriviaRange const* trivia, CharacterLiteralValue const* value)
            : _kind{TokenKind::CharacterLiteral}
            , _source{source}
            , _trivia{trivia}
            , _data{value}
        {
        }

        Token(source::SourceSpan const& source, TriviaRange const* trivia, Identifier const* value)
            : _kind{TokenKind::Identifier}
            , _source{source}
            , _trivia{trivia}
            , _data{value}
        {
        }

        [[nodiscard]] TokenKind GetKind() const
        {
            return this->_kind;
        }

        [[nodiscard]] source::SourceSpan const& GetSource() const
        {
            return this->_source;
        }

        [[nodiscard]] std::span<Trivia const> GetLeadingTrivia() const
        {
            return this->_trivia->Leading;
        }

        [[nodiscard]] std::span<Trivia const> GetTrailingTrivia() const
        {
            return this->_trivia->Trailing;
        }

        [[nodiscard]] constexpr IntegerLiteralValue const* TryGetIntegerValue() const
        {
            if (this->_kind == TokenKind::IntegerLiteral)
            {
                return static_cast<IntegerLiteralValue const*>(this->_data);
            }

            return nullptr;
        }

        [[nodiscard]] constexpr FloatLiteralValue const* TryGetFloatValue() const
        {
            if (this->_kind == TokenKind::FloatLiteral)
            {
                return static_cast<FloatLiteralValue const*>(this->_data);
            }

            return nullptr;
        }

        [[nodiscard]] constexpr StringLiteralValue const* TryGetStringValue() const
        {
            if (this->_kind == TokenKind::StringLiteral)
            {
                return static_cast<StringLiteralValue const*>(this->_data);
            }

            return nullptr;
        }

        [[nodiscard]] constexpr CharacterLiteralValue const* TryGetCharacterValue() const
        {
            if (this->_kind == TokenKind::CharacterLiteral)
            {
                return static_cast<CharacterLiteralValue const*>(this->_data);
            }

            return nullptr;
        }

        [[nodiscard]] IntegerLiteralValue const& GetIntegerValue() const
        {
            WEAVE_ASSERT(this->_kind == TokenKind::IntegerLiteral);
            return *static_cast<IntegerLiteralValue const*>(this->_data);
        }

        [[nodiscard]] FloatLiteralValue const& GetFloatValue() const
        {
            WEAVE_ASSERT(this->_kind == TokenKind::FloatLiteral);
            return *static_cast<FloatLiteralValue const*>(this->_data);
        }

        [[nodiscard]] StringLiteralValue const& GetStringValue() const
        {
            WEAVE_ASSERT(this->_kind == TokenKind::StringLiteral);
            return *static_cast<StringLiteralValue const*>(this->_data);
        }

        [[nodiscard]] CharacterLiteralValue const& GetCharacterValue() const
        {
            WEAVE_ASSERT(this->_kind == TokenKind::CharacterLiteral);
            return *static_cast<CharacterLiteralValue const*>(this->_data);
        }
    };
}
