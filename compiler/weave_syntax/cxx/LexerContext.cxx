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

                [[maybe_unused]] unicode::UnicodeConversionResult const status = unicode::utf8_decode(value, first, last);

                WEAVE_ASSERT(status == unicode::UnicodeConversionResult::Success);
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

    Token* LexerContext::Create(
        TokenKind kind,
        source::SourceSpan const& source)
    {
        return this->Tokens.create(
            kind,
            source);
    }

    Token* LexerContext::Create(
        TokenKind kind,
        source::SourceSpan const& source,
        std::span<Trivia const> leadingTrivia,
        std::span<Trivia const> trailingTrivia)
    {
        return this->Create(
            kind,
            source,
            this->Trivias.create_array(leadingTrivia),
            this->Trivias.create_array(trailingTrivia),
            nullptr);
    }

    Token* LexerContext::Create(
        TokenKind kind,
        source::SourceSpan const& source,
        std::span<Trivia const> leadingTrivia,
        std::span<Trivia const> trailingTrivia,
        void* value)
    {
        WEAVE_ASSERT(leadingTrivia.size() < std::numeric_limits<uint16_t>::max());
        WEAVE_ASSERT(trailingTrivia.size() < std::numeric_limits<uint16_t>::max());

        Trivia const* trivia = nullptr;

        if (size_t const count = leadingTrivia.size() + trailingTrivia.size(); count != 0)
        {
            trivia = this->Trivias.create_copy_combined(leadingTrivia, trailingTrivia).data();
        }

        return this->Tokens.create(
            kind,
            source,
            trivia,
            static_cast<uint16_t>(leadingTrivia.size()),
            static_cast<uint16_t>(trailingTrivia.size()),
            value);
    }

    Token* LexerContext::CreateMissing(
        TokenKind kind,
        source::SourceSpan const& source)
    {
        return this->MissingTokens.create(
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
        Token* result = this->Create(
            TokenKind::CharacterLiteral,
            source,
            leadingTrivia,
            trailingTrivia,
            this->CharacterLiterals.create(prefix, value));
        return result;
    }

    Token* LexerContext::CreateString(
        source::SourceSpan const& source,
        std::span<Trivia const> leadingTrivia,
        std::span<Trivia const> trailingTrivia,
        StringPrefixKind prefix,
        std::string_view value)
    {
        Token* result = this->Create(
            TokenKind::StringLiteral,
            source,
            leadingTrivia,
            trailingTrivia,
            this->StringLiterals.create(prefix, this->Strings.get(value)));
        return result;
    }

    Token* LexerContext::CreateFloat(
        source::SourceSpan const& source,
        std::span<Trivia const> leadingTrivia,
        std::span<Trivia const> trailingTrivia,
        NumberLiteralPrefixKind prefix,
        std::string_view value,
        FloatLiteralSuffixKind suffix)
    {
        Token* result = this->Create(
            TokenKind::FloatLiteral,
            source,
            leadingTrivia,
            trailingTrivia,
            this->FloatLiterals.create(prefix, suffix, this->Strings.get(value)));
        return result;
    }

    Token* LexerContext::CreateInteger(
        source::SourceSpan const& source,
        std::span<Trivia const> leadingTrivia,
        std::span<Trivia const> trailingTrivia,
        NumberLiteralPrefixKind prefix,
        std::string_view value,
        IntegerLiteralSuffixKind suffix)
    {
        Token* result = this->Create(
            TokenKind::IntegerLiteral,
            source,
            leadingTrivia,
            trailingTrivia,
            this->IntegerLiterals.create(prefix, suffix, this->Strings.get(value)));
        return result;
    }

    Token* LexerContext::CreateIdentifier(
        source::SourceSpan const& source,
        std::span<Trivia const> leadingTrivia,
        std::span<Trivia const> trailingTrivia,
        std::string_view value)
    {
        Token* result = this->Create(
            TokenKind::Identifier,
            source,
            leadingTrivia,
            trailingTrivia,
            this->Identifiers.create(this->Strings.get(value)));
        return result;
    }

    void LexerContext::QueryMemoryUsage(size_t& allocated, size_t& reserved) const
    {
        allocated = 0;
        reserved = 0;

        this->Tokens.query_memory_usage(allocated, reserved);
        this->MissingTokens.query_memory_usage(allocated, reserved);
        this->Trivias.query_memory_usage(allocated, reserved);
        this->CharacterLiterals.query_memory_usage(allocated, reserved);
        this->StringLiterals.query_memory_usage(allocated, reserved);
        this->FloatLiterals.query_memory_usage(allocated, reserved);
        this->IntegerLiterals.query_memory_usage(allocated, reserved);
        this->Identifiers.query_memory_usage(allocated, reserved);

        this->Strings.query_memory_usage(allocated, reserved);
    }

    void LexerContext::DumpMemoryUsage() const
    {
        {
            size_t allocated = 0;
            size_t reserved = 0;
            this->Tokens.query_memory_usage(allocated, reserved);
            fmt::println("Tokens:               allocated = {}, reserved = {}", allocated, reserved);
        }
        {
            size_t allocated = 0;
            size_t reserved = 0;
            this->MissingTokens.query_memory_usage(allocated, reserved);
            fmt::println("MissingTokens:        allocated = {}, reserved = {}", allocated, reserved);
        }
        {
            size_t allocated = 0;
            size_t reserved = 0;
            this->Trivias.query_memory_usage(allocated, reserved);
            fmt::println("Trivias:              allocated = {}, reserved = {}", allocated, reserved);
        }
        {
            size_t allocated = 0;
            size_t reserved = 0;
            this->CharacterLiterals.query_memory_usage(allocated, reserved);
            fmt::println("CharacterLiterals:    allocated = {}, reserved = {}", allocated, reserved);
        }
        {
            size_t allocated = 0;
            size_t reserved = 0;
            this->StringLiterals.query_memory_usage(allocated, reserved);
            fmt::println("StringLiterals:       allocated = {}, reserved = {}", allocated, reserved);
        }
        {
            size_t allocated = 0;
            size_t reserved = 0;
            this->FloatLiterals.query_memory_usage(allocated, reserved);
            fmt::println("FloatLiterals:        allocated = {}, reserved = {}", allocated, reserved);
        }
        {
            size_t allocated = 0;
            size_t reserved = 0;
            this->IntegerLiterals.query_memory_usage(allocated, reserved);
            fmt::println("IntegerLiterals:      allocated = {}, reserved = {}", allocated, reserved);
        }
        {
            size_t allocated = 0;
            size_t reserved = 0;
            this->Identifiers.query_memory_usage(allocated, reserved);
            fmt::println("Identifiers:          allocated = {}, reserved = {}", allocated, reserved);
        }
        {
            size_t allocated = 0;
            size_t reserved = 0;
            this->Strings.query_memory_usage(allocated, reserved);
            fmt::println("Strings:              allocated = {}, reserved = {}", allocated, reserved);
        }
    }
}
