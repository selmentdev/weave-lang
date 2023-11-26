#include "weave/tokenizer/TokenizerContext.hxx"
#include "weave/tokenizer/Tokenizer.hxx"
#include "weave/Unicode.hxx"

namespace weave::tokenizer
{
    Token* TokenizerContext::Lex(Tokenizer& lexer)
    {
        if (lexer.Lex())
        {
            if (lexer.GetKind() == TokenKind::StringLiteral)
            {
                return this->CreateString(
                    lexer.GetSpan(),
                    lexer.GetLeadingTrivia(),
                    lexer.GetTrailingTrivia(),
                    StringPrefixKind::Default,
                    lexer.GetValue());
            }

            if (lexer.GetKind() == TokenKind::CharacterLiteral)
            {
                char32_t value{};
                std::string_view const s = lexer.GetValue();

                const char* first = s.data();
                const char* const last = first + s.size();

                [[maybe_unused]] unicode::ConversionResult const status = unicode::Decode(value, first, last);

                WEAVE_ASSERT(status == unicode::ConversionResult::Success);
                WEAVE_ASSERT(first == last);
                WEAVE_ASSERT(first[0] == '\0');

                return this->CreateCharacter(
                    lexer.GetSpan(),
                    lexer.GetLeadingTrivia(),
                    lexer.GetTrailingTrivia(),
                    CharacterPrefixKind::Default,
                    value);
            }

            if (lexer.GetKind() == TokenKind::FloatLiteral)
            {
                return this->CreateFloat(
                    lexer.GetSpan(),
                    lexer.GetLeadingTrivia(),
                    lexer.GetTrailingTrivia(),
                    NumberLiteralPrefixKind::Default,
                    lexer.GetValue(),
                    FloatLiteralSuffixKind::Default);
            }

            if (lexer.GetKind() == TokenKind::IntegerLiteral)
            {
                return this->CreateInteger(
                    lexer.GetSpan(),
                    lexer.GetLeadingTrivia(),
                    lexer.GetTrailingTrivia(),
                    NumberLiteralPrefixKind::Default,
                    lexer.GetValue(),
                    IntegerLiteralSuffixKind::Default);
            }

            if (lexer.GetKind() == TokenKind::Identifier)
            {
                return this->CreateIdentifier(
                    lexer.GetSpan(),
                    lexer.GetLeadingTrivia(),
                    lexer.GetTrailingTrivia(),
                    lexer.GetValue());
            }

            return this->Create(
                lexer.GetKind(),
                lexer.GetSpan(),
                lexer.GetLeadingTrivia(),
                lexer.GetTrailingTrivia());
        }

        // Failed to lex token
        return this->Create(
            TokenKind::Error,
            lexer.GetSpan());
    }

    TriviaRange* TokenizerContext::CreateTriviaRange(std::span<Trivia const> leading, std::span<Trivia const> trailing)
    {
        if (leading.empty() and trailing.empty())
        {
            return &this->_empty_trivia;
        }

        TriviaRange const result{
            .Leading = this->Trivias.EmplaceArray(leading),
            .Trailing = this->Trivias.EmplaceArray(trailing),
        };

        return this->TriviaRanges.Emplace(result);
    }

    Token* TokenizerContext::Create(
        TokenKind kind,
        source::SourceSpan const& source)
    {
        return this->Tokens.Emplace(
            kind,
            source,
            &this->_empty_trivia);
    }

    Token* TokenizerContext::Create(
        TokenKind kind,
        source::SourceSpan const& source,
        std::span<Trivia const> leadingTrivia,
        std::span<Trivia const> trailingTrivia)
    {
        WEAVE_ASSERT(kind != TokenKind::Identifier);
        WEAVE_ASSERT(kind != TokenKind::IntegerLiteral);
        WEAVE_ASSERT(kind != TokenKind::FloatLiteral);
        WEAVE_ASSERT(kind != TokenKind::StringLiteral);
        WEAVE_ASSERT(kind != TokenKind::CharacterLiteral);

        return this->Tokens.Emplace(
            kind,
            source,
            this->CreateTriviaRange(leadingTrivia, trailingTrivia));
    }

    Token* TokenizerContext::CreateMissing(
        TokenKind kind,
        source::SourceSpan const& source)
    {
        return this->Tokens.Emplace(
            kind,
            source,
            &this->_empty_trivia);
    }

    CharacterLiteralToken* TokenizerContext::CreateCharacter(
        source::SourceSpan const& source,
        std::span<Trivia const> leadingTrivia,
        std::span<Trivia const> trailingTrivia,
        CharacterPrefixKind prefix,
        char32_t value)
    {
        return this->CharacterLiterals.Emplace(
            source,
            this->CreateTriviaRange(leadingTrivia, trailingTrivia),
            prefix,
            value);
    }

    StringLiteralToken* TokenizerContext::CreateString(
        source::SourceSpan const& source,
        std::span<Trivia const> leadingTrivia,
        std::span<Trivia const> trailingTrivia,
        StringPrefixKind prefix,
        std::string_view value)
    {
        return this->StringLiterals.Emplace(
            source,
            this->CreateTriviaRange(leadingTrivia, trailingTrivia),
            prefix,
            value);
    }

    FloatLiteralToken* TokenizerContext::CreateFloat(
        source::SourceSpan const& source,
        std::span<Trivia const> leadingTrivia,
        std::span<Trivia const> trailingTrivia,
        NumberLiteralPrefixKind prefix,
        std::string_view value,
        FloatLiteralSuffixKind suffix)
    {
        return this->FloatLiterals.Emplace(
            source,
            this->CreateTriviaRange(leadingTrivia, trailingTrivia),
            prefix,
            suffix,
            value);
    }

    IntegerLiteralToken* TokenizerContext::CreateInteger(
        source::SourceSpan const& source,
        std::span<Trivia const> leadingTrivia,
        std::span<Trivia const> trailingTrivia,
        NumberLiteralPrefixKind prefix,
        std::string_view value,
        IntegerLiteralSuffixKind suffix)
    {
        return this->IntegerLiterals.Emplace(
            source,
            this->CreateTriviaRange(leadingTrivia, trailingTrivia),
            prefix,
            suffix,
            value);
    }

    IdentifierToken* TokenizerContext::CreateIdentifier(
        source::SourceSpan const& source,
        std::span<Trivia const> leadingTrivia,
        std::span<Trivia const> trailingTrivia,
        std::string_view value)
    {
        return this->Identifiers.Emplace(
            source,
            this->CreateTriviaRange(leadingTrivia, trailingTrivia),
            value);
    }

    void TokenizerContext::QueryMemoryUsage(size_t& allocated, size_t& reserved) const
    {
        allocated = 0;
        reserved = 0;

        this->Tokens.QueryMemoryUsage(allocated, reserved);
        this->TriviaRanges.QueryMemoryUsage(allocated, reserved);
        this->Trivias.QueryMemoryUsage(allocated, reserved);
        this->CharacterLiterals.QueryMemoryUsage(allocated, reserved);
        this->StringLiterals.QueryMemoryUsage(allocated, reserved);
        this->FloatLiterals.QueryMemoryUsage(allocated, reserved);
        this->IntegerLiterals.QueryMemoryUsage(allocated, reserved);
        this->Identifiers.QueryMemoryUsage(allocated, reserved);

        this->Strings.QueryMemoryUsage(allocated, reserved);
    }

    void TokenizerContext::DumpMemoryUsage() const
    {
        {
            size_t allocated = 0;
            size_t reserved = 0;
            this->Tokens.QueryMemoryUsage(allocated, reserved);
            fmt::println("Tokens:               allocated = {}, reserved = {}", allocated, reserved);
        }
        {
            size_t allocated = 0;
            size_t reserved = 0;
            this->TriviaRanges.QueryMemoryUsage(allocated, reserved);
            fmt::println("TriviaRanges:         allocated = {}, reserved = {}", allocated, reserved);
        }
        {
            size_t allocated = 0;
            size_t reserved = 0;
            this->Trivias.QueryMemoryUsage(allocated, reserved);
            fmt::println("Trivias:              allocated = {}, reserved = {}", allocated, reserved);
        }
        {
            size_t allocated = 0;
            size_t reserved = 0;
            this->CharacterLiterals.QueryMemoryUsage(allocated, reserved);
            fmt::println("CharacterLiterals:    allocated = {}, reserved = {}", allocated, reserved);
        }
        {
            size_t allocated = 0;
            size_t reserved = 0;
            this->StringLiterals.QueryMemoryUsage(allocated, reserved);
            fmt::println("StringLiterals:       allocated = {}, reserved = {}", allocated, reserved);
        }
        {
            size_t allocated = 0;
            size_t reserved = 0;
            this->FloatLiterals.QueryMemoryUsage(allocated, reserved);
            fmt::println("FloatLiterals:        allocated = {}, reserved = {}", allocated, reserved);
        }
        {
            size_t allocated = 0;
            size_t reserved = 0;
            this->IntegerLiterals.QueryMemoryUsage(allocated, reserved);
            fmt::println("IntegerLiterals:      allocated = {}, reserved = {}", allocated, reserved);
        }
        {
            size_t allocated = 0;
            size_t reserved = 0;
            this->Identifiers.QueryMemoryUsage(allocated, reserved);
            fmt::println("Identifiers:          allocated = {}, reserved = {}", allocated, reserved);
        }
        {
            size_t allocated = 0;
            size_t reserved = 0;
            this->Strings.QueryMemoryUsage(allocated, reserved);
            fmt::println("Strings:              allocated = {}, reserved = {}", allocated, reserved);
        }
    }
}
