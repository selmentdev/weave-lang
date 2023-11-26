#include "weave/syntax/LexerContext.hxx"
#include "weave/syntax/Lexer.hxx"
#include "weave/Unicode.hxx"

namespace weave::syntax
{
    Token* LexerContext::Lex(Lexer& lexer)
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

    TriviaRange* LexerContext::CreateTriviaRange(std::span<Trivia const> leading, std::span<Trivia const> trailing)
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

    Token* LexerContext::Create(
        TokenKind kind,
        source::SourceSpan const& source)
    {
        return this->Tokens.Emplace(
            kind,
            source);
    }

    Token* LexerContext::Create(
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

    Token* LexerContext::CreateMissing(
        TokenKind kind,
        source::SourceSpan const& source)
    {
        return this->MissingTokens.Emplace(
            kind,
            source);
    }

    Token* LexerContext::CreateCharacter(
        source::SourceSpan const& source,
        std::span<Trivia const> leadingTrivia,
        std::span<Trivia const> trailingTrivia,
        CharacterPrefixKind prefix,
        char32_t value)
    {
        return this->Tokens.Emplace(
            source,
            this->CreateTriviaRange(leadingTrivia, trailingTrivia),
            this->CharacterLiterals.Emplace(prefix, value));
    }

    Token* LexerContext::CreateString(
        source::SourceSpan const& source,
        std::span<Trivia const> leadingTrivia,
        std::span<Trivia const> trailingTrivia,
        StringPrefixKind prefix,
        std::string_view value)
    {
        return this->Tokens.Emplace(
            source,
            this->CreateTriviaRange(leadingTrivia, trailingTrivia),
            this->StringLiterals.Emplace(prefix, value));
    }

    Token* LexerContext::CreateFloat(
        source::SourceSpan const& source,
        std::span<Trivia const> leadingTrivia,
        std::span<Trivia const> trailingTrivia,
        NumberLiteralPrefixKind prefix,
        std::string_view value,
        FloatLiteralSuffixKind suffix)
    {
        return this->Tokens.Emplace(
            source,
            this->CreateTriviaRange(leadingTrivia, trailingTrivia),
            this->FloatLiterals.Emplace(prefix, suffix, value));
    }

    Token* LexerContext::CreateInteger(
        source::SourceSpan const& source,
        std::span<Trivia const> leadingTrivia,
        std::span<Trivia const> trailingTrivia,
        NumberLiteralPrefixKind prefix,
        std::string_view value,
        IntegerLiteralSuffixKind suffix)
    {
        return this->Tokens.Emplace(
            source,
            this->CreateTriviaRange(leadingTrivia, trailingTrivia),
            this->IntegerLiterals.Emplace(prefix, suffix, value));
    }

    Token* LexerContext::CreateIdentifier(
        source::SourceSpan const& source,
        std::span<Trivia const> leadingTrivia,
        std::span<Trivia const> trailingTrivia,
        std::string_view value)
    {
        return this->Tokens.Emplace(
            source,
            this->CreateTriviaRange(leadingTrivia, trailingTrivia),
            this->Identifiers.Emplace(value));
    }

    void LexerContext::QueryMemoryUsage(size_t& allocated, size_t& reserved) const
    {
        allocated = 0;
        reserved = 0;

        this->Tokens.QueryMemoryUsage(allocated, reserved);
        this->MissingTokens.QueryMemoryUsage(allocated, reserved);
        this->TriviaRanges.QueryMemoryUsage(allocated, reserved);
        this->Trivias.QueryMemoryUsage(allocated, reserved);
        this->CharacterLiterals.QueryMemoryUsage(allocated, reserved);
        this->StringLiterals.QueryMemoryUsage(allocated, reserved);
        this->FloatLiterals.QueryMemoryUsage(allocated, reserved);
        this->IntegerLiterals.QueryMemoryUsage(allocated, reserved);
        this->Identifiers.QueryMemoryUsage(allocated, reserved);

        this->Strings.QueryMemoryUsage(allocated, reserved);
    }

    void LexerContext::DumpMemoryUsage() const
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
            this->MissingTokens.QueryMemoryUsage(allocated, reserved);
            fmt::println("MissingTokens:        allocated = {}, reserved = {}", allocated, reserved);
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
