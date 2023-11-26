#pragma once
#include "weave/BugCheck.hxx"
#include "weave/source/Source.hxx"
#include "weave/lexer/TokenKind.hxx"
#include "weave/lexer/TriviaKind.hxx"

#include <span>

// Design:
//
// - SyntaxContext - a place to hold state, allocations etc.
// -*

namespace weave::lexer
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

    class Token
    {
    private:
        TokenKind _kind;
        source::SourceSpan _source;
        TriviaRange const* _trivia;

    public:
        constexpr Token(TokenKind kind, source::SourceSpan const& source, TriviaRange const* trivia)
            : _kind{kind}
            , _source{source}
            , _trivia{trivia}
        {
        }

    public:
        [[nodiscard]] constexpr TokenKind GetKind() const
        {
            return this->_kind;
        }

        [[nodiscard]] constexpr source::SourceSpan const& GetSourceSpan() const
        {
            return this->_source;
        }

        [[nodiscard]] constexpr std::span<Trivia const> GetLeadingTrivia() const
        {
            if (this->_trivia != nullptr)
            {
                return this->_trivia->Leading;
            }

            return {};
        }

        [[nodiscard]] constexpr std::span<Trivia const> GetTrailingTrivia() const
        {
            if (this->_trivia != nullptr)
            {
                return this->_trivia->Trailing;
            }

            return {};
        }

    public:
        [[nodiscard]] constexpr bool Is(TokenKind kind) const
        {
            return this->_kind == kind;
        }

        template <typename T>
        [[nodiscard]] constexpr T const* TryCast() const
        {
            if (this->_kind == T::Kind)
            {
                return static_cast<T const*>(this);
            }

            return nullptr;
        }
    };

    class IntegerLiteralToken final : public Token
    {
    public:
        static constexpr TokenKind Kind = TokenKind::IntegerLiteral;

    private:
        NumberLiteralPrefixKind _prefix;
        IntegerLiteralSuffixKind _suffix;
        std::string_view _value;

    public:
        constexpr IntegerLiteralToken(
            source::SourceSpan const& source,
            TriviaRange const* trivia,
            NumberLiteralPrefixKind prefix,
            IntegerLiteralSuffixKind suffix,
            std::string_view value)
            : Token{TokenKind::IntegerLiteral, source, trivia}
            , _prefix{prefix}
            , _suffix{suffix}
            , _value{value}
        {
        }

    public:
        [[nodiscard]] constexpr NumberLiteralPrefixKind GetPrefix() const
        {
            return this->_prefix;
        }

        [[nodiscard]] constexpr IntegerLiteralSuffixKind GetSuffix() const
        {
            return this->_suffix;
        }

        [[nodiscard]] constexpr std::string_view GetValue() const
        {
            return this->_value;
        }
    };

    class FloatLiteralToken final : public Token
    {
    public:
        static constexpr TokenKind Kind = TokenKind::FloatLiteral;

    private:
        NumberLiteralPrefixKind _prefix;
        FloatLiteralSuffixKind _suffix;
        std::string_view _value;

    public:
        constexpr FloatLiteralToken(
            source::SourceSpan const& source,
            TriviaRange const* trivia,
            NumberLiteralPrefixKind prefix,
            FloatLiteralSuffixKind suffix,
            std::string_view value)
            : Token{TokenKind::FloatLiteral, source, trivia}
            , _prefix{prefix}
            , _suffix{suffix}
            , _value{value}
        {
        }

    public:
        [[nodiscard]] constexpr NumberLiteralPrefixKind GetPrefix() const
        {
            return this->_prefix;
        }

        [[nodiscard]] constexpr FloatLiteralSuffixKind GetSuffix() const
        {
            return this->_suffix;
        }

        [[nodiscard]] constexpr std::string_view GetValue() const
        {
            return this->_value;
        }
    };

    class StringLiteralToken final : public Token
    {
    public:
        static constexpr TokenKind Kind = TokenKind::StringLiteral;

    private:
        StringPrefixKind _prefix;
        std::string_view _value;

    public:
        constexpr StringLiteralToken(
            source::SourceSpan const& source,
            TriviaRange const* trivia,
            StringPrefixKind prefix,
            std::string_view value)
            : Token{TokenKind::StringLiteral, source, trivia}
            , _prefix{prefix}
            , _value{value}
        {
        }

    public:
        [[nodiscard]] constexpr StringPrefixKind GetPrefix() const
        {
            return this->_prefix;
        }

        [[nodiscard]] constexpr std::string_view GetValue() const
        {
            return this->_value;
        }
    };

    class CharacterLiteralToken final : public Token
    {
    public:
        static constexpr TokenKind Kind = TokenKind::CharacterLiteral;

    private:
        CharacterPrefixKind _prefix;
        char32_t _value;

    public:
        constexpr CharacterLiteralToken(
            source::SourceSpan const& source,
            TriviaRange const* trivia,
            CharacterPrefixKind prefix,
            char32_t value)
            : Token{TokenKind::CharacterLiteral, source, trivia}
            , _prefix{prefix}
            , _value{value}
        {
        }

    public:
        [[nodiscard]] constexpr CharacterPrefixKind GetPrefix() const
        {
            return this->_prefix;
        }

        [[nodiscard]] constexpr char32_t GetValue() const
        {
            return this->_value;
        }
    };

    class IdentifierToken final : public Token
    {
    public:
        static constexpr TokenKind Kind = TokenKind::Identifier;

    private:
        std::string_view _identifier;

    public:
        constexpr IdentifierToken(
            source::SourceSpan const& source,
            TriviaRange const* trivia,
            std::string_view identifier)
            : Token{TokenKind::Identifier, source, trivia}
            , _identifier{identifier}
        {
        }

    public:
        [[nodiscard]] constexpr std::string_view GetIdentifier() const
        {
            return this->_identifier;
        }
    };
}
