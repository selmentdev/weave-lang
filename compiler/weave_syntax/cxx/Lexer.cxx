#include "weave/syntax/Lexer.hxx"
#include "weave/syntax/CharTraits.hxx"
#include "weave/syntax/SyntaxFactory.hxx"
#include "weave/Unicode.hxx"

#include <array>

namespace weave::syntax::impl
{
    constexpr bool AppendDecCharacter(uint64_t& result, char32_t c)
    {
        if ((U'0' <= c) and (c <= U'9'))
        {
            uint64_t const digit = (c - U'0');

            result *= 10;
            result += digit;
            return true;
        }

        return false;
    }

    constexpr bool AppendHexCharacter(uint64_t& result, char32_t c)
    {
        uint64_t digit = 16;

        if ((U'0' <= c) and (c <= U'9'))
        {
            digit = (c - U'0');
        }
        else if ((U'a' <= c) and (c <= U'f'))
        {
            digit = (c - U'a') + 10;
        }
        else if ((U'A' <= c) and (c <= U'F'))
        {
            digit = (c - U'A') + 10;
        }

        bool const valid = digit < 16;

        if (valid)
        {
            result <<= 4u;
            result |= digit;
        }

        return valid;
    }
}

namespace weave::syntax
{
    bool Lexer::Lex(TokenInfo& token)
    {
        this->TryReadTrivia(token.LeadingTrivia, true);

        bool const success = this->TryReadToken(token);

        this->TryReadTrivia(token.TrailingTrivia, false);

        return success;
    }

    SyntaxToken* Lexer::Lex(SyntaxFactory& factory)
    {
        // TODO: Determine how to parse token prefix and postfix for each literal type
        // Fix proposal:
        //  - have common prefix and postfix for all literals, which will be converted here to proper type (type-safe, scope limited to just lexer)
        //  - have all possible prefixes and postfixes as common type and use it everywhere (not type-safe)

        if (this->Lex(this->_token))
        {
            if (this->_token.Kind == SyntaxKind::StringLiteralToken)
            {
                return factory.CreateStringLiteralToken(
                    this->_token.Source,
                    this->_token.LeadingTrivia,
                    this->_token.TrailingTrivia,
                    this->_token.Prefix,
                    this->_token.Value);
            }

            if (this->_token.Kind == SyntaxKind::CharacterLiteralToken)
            {
                char32_t value{};
                std::string_view const s = this->_token.Value;

                const char* first = s.data();
                const char* const last = first + s.size();

                // Ignore decoding errors - they will be reported by the tokenizer.
                (void)unicode::Decode(value, first, last);

                return factory.CreateCharacterLiteralToken(
                    this->_token.Source,
                    this->_token.LeadingTrivia,
                    this->_token.TrailingTrivia,
                    this->_token.Prefix,
                    value);
            }

            if (this->_token.Kind == SyntaxKind::FloatLiteralToken)
            {
                return factory.CreateFloatLiteralToken(
                    this->_token.Source,
                    this->_token.LeadingTrivia,
                    this->_token.TrailingTrivia,
                    this->_token.Prefix,
                    this->_token.Suffix,
                    this->_token.Value);
            }

            if (this->_token.Kind == SyntaxKind::IntegerLiteralToken)
            {
                return factory.CreateIntegerLiteralToken(
                    this->_token.Source,
                    this->_token.LeadingTrivia,
                    this->_token.TrailingTrivia,
                    this->_token.Prefix,
                    this->_token.Suffix,
                    this->_token.Value);
            }

            if (this->_token.Kind == SyntaxKind::IdentifierToken)
            {
                return factory.CreateIdentifierToken(
                    this->_token.Source,
                    this->_token.LeadingTrivia,
                    this->_token.TrailingTrivia,
                    this->_token.ContextualKeyword,
                    this->_token.Value);
            }

            return factory.CreateToken(
                this->_token.Kind,
                this->_token.Source,
                this->_token.LeadingTrivia,
                this->_token.TrailingTrivia);
        }

        // Failed to lex token
        return factory.CreateToken(
            SyntaxKind::None,
            this->_token.Source);
    }

    bool Lexer::TryReadToken(TokenInfo& token)
    {
        this->_cursor.Start();

        token.Prefix = LiteralPrefixKind::Default;
        token.Value.clear();
        token.Suffix.clear();

        if (this->_cursor.IsEnd())
        {
            token.Kind = SyntaxKind::EndOfFileToken;
            token.Source = this->_cursor.GetSpan();
            return true;
        }

        if (this->TryReadRawIdentifier(token))
        {
            token.Kind = SyntaxKind::IdentifierToken;
            token.Source = this->_cursor.GetSpan();
            return true;
        }

        if (this->TryReadStringLiteral(token))
        {
            token.Kind = SyntaxKind::StringLiteralToken;
            token.Source = this->_cursor.GetSpan();
            return true;
        }

        if (this->TryReadCharacterLiteral(token))
        {
            token.Kind = SyntaxKind::CharacterLiteralToken;
            token.Source = this->_cursor.GetSpan();
            return true;
        }

        if (this->TryReadNumericLiteral(token))
        {
            token.Source = this->_cursor.GetSpan();
            return true;
        }

        if (this->TryReadPunctuation(token))
        {
            token.Source = this->_cursor.GetSpan();
            return true;
        }

        if (this->TryReadIdentifier(token))
        {
            if (std::optional<SyntaxKind> const keyword = TryMapIdentifierToKeyword(token.Value); keyword.has_value())
            {
                if (IsContextualKeyword(*keyword))
                {
                    token.Kind = SyntaxKind::IdentifierToken;
                    token.ContextualKeyword = *keyword;
                }
                else
                {
                    token.Kind = *keyword;
                    token.ContextualKeyword = SyntaxKind::None;
                }
            }
            else
            {
                if ((token.Value.length() == 1) and (token.Value.front() == '_'))
                {
                    token.Kind = SyntaxKind::UnderscoreToken;
                }
                else
                {
                    token.Kind = SyntaxKind::IdentifierToken;
                }

                token.ContextualKeyword = SyntaxKind::None;
            }

            token.Source = this->_cursor.GetSpan();
            return true;
        }

        if (this->_cursor.IsValid())
        {
            // Current character is not recognized.
            this->_diagnostic->AddError(this->_cursor.GetSpan(), "unexpected character");
        }
        else
        {
            // Character was not encoded in UTF8. This is not a EOF.
            this->_diagnostic->AddError(this->_cursor.GetSpan(), "invalid UTF-8 character");
        }

        token.Kind = SyntaxKind::None;
        token.Source = this->_cursor.GetSpan();
        return false;
    }

    void Lexer::TryReadTrivia(std::vector<SyntaxTrivia>& builder, bool leading)
    {
        builder.clear();

        while (true)
        {
            if (this->_cursor.IsEnd())
            {
                break;
            }

            this->_cursor.Start();

            if (this->TryReadEndOfLine())
            {
                if (this->_mode == LexerTriviaMode::All)
                {
                    builder.emplace_back(SyntaxKind::EndOfLineTrivia, this->_cursor.GetSpan());
                }

                if (not leading)
                {
                    break;
                }
            }
            else if (this->TryReadWhitespace())
            {
                if (this->_mode == LexerTriviaMode::All)
                {
                    builder.emplace_back(SyntaxKind::WhitespaceTrivia, this->_cursor.GetSpan());
                }
            }
            else if (SyntaxKind kind = this->TryReadComment(); kind != SyntaxKind::None)
            {
                if ((this->_mode == LexerTriviaMode::All) or ((this->_mode == LexerTriviaMode::Documentation) and IsDocumentationTrivia(kind)))
                {
                    builder.emplace_back(kind, this->_cursor.GetSpan());
                }
            }
            else
            {
                break;
            }
        }
    }

    bool Lexer::TryReadRawIdentifier(TokenInfo& token)
    {
        source::SourcePosition const start = this->_cursor.GetCurrentPosition();

        if (this->_cursor.First(U'r'))
        {
            if (this->_cursor.First(U'#'))
            {
                if (this->TryReadIdentifier(token))
                {
                    return true;
                }
            }
        }

        this->_cursor.Reset(start);
        return false;
    }

    bool Lexer::TryReadRawStringLiteral(TokenInfo& token)
    {
        source::SourcePosition const start = this->_cursor.GetCurrentPosition();

        if (this->_cursor.First(U'r'))
        {
            // Count number of '#' before literal.
            size_t const delimiter = this->_cursor.Count(U'#');

            if (this->_cursor.First(U'"'))
            {
                bool terminated = false;

                while (true)
                {
                    if (not this->_cursor.IsValid() or this->_cursor.IsEnd())
                    {
                        break;
                    }

                    if (this->_cursor.Peek() == U'"')
                    {
                        source::SourcePosition const startTerminator = this->_cursor.GetCurrentPosition();
                        this->_cursor.Advance();

                        size_t const matched = this->_cursor.Count(U'#');

                        if (matched == delimiter)
                        {
                            // Consume '"#...#' and stop.
                            terminated = true;
                            break;
                        }

                        if (matched > delimiter)
                        {
                            this->_diagnostic->AddError(
                                this->_cursor.GetSpanToCurrent(startTerminator),
                                "raw string literal terminator too long");

                            terminated = true;
                            break;
                        }

                        // Append unmatched characters.
                        token.Value.push_back('"');
                        token.Value.append(matched, '#');
                    }
                    else
                    {
                        // Append whatever we decoded.
                        token.Value.append(this->_cursor.PeekAsStringView());
                        this->_cursor.Advance();
                    }
                }

                if (not terminated)
                {
                    this->_diagnostic->AddError(
                        this->_cursor.GetSpan(),
                        "missing terminating character error");
                }

                return terminated;
            }
        }

        this->_cursor.Reset(start);
        return false;
    }

    bool Lexer::TryReadDefaultStringLiteral(TokenInfo& token)
    {
        size_t consumed{};
        if (this->TryReadStringOrCharacterLiteralCore(token.Value, U'"', consumed))
        {
            return true;
        }

        return false;
    }

    bool Lexer::TryReadStringLiteral(TokenInfo& token)
    {
        source::SourcePosition const start = this->_cursor.GetCurrentPosition();

        LiteralPrefixKind const prefix = this->TryReadStringPrefix();

        if (this->TryReadRawStringLiteral(token) or this->TryReadDefaultStringLiteral(token))
        {
            token.Prefix = prefix;
            return true;
        }

        // Not a string literal.
        this->_cursor.Reset(start);
        return false;
    }

    bool Lexer::TryReadCharacterLiteral(TokenInfo& token)
    {
        size_t consumed{};

        if (this->TryReadStringOrCharacterLiteralCore(token.Value, U'\'', consumed))
        {
            if (consumed == 0)
            {
                this->_diagnostic->AddError(this->_cursor.GetSpan(), "empty character literal");
            }
            else if (consumed > 1)
            {
                this->_diagnostic->AddError(this->_cursor.GetSpan(), "character literal may only contain one codepoint");
            }

            this->TryReadLiteralSuffix(token.Suffix);

            return true;
        }

        return false;
    }

    bool Lexer::TryReadNumericLiteral(TokenInfo& token)
    {
        if (not CharTraits::IsDecimalDigit(this->_cursor.Peek()))
        {
            return false;
        }

        int radix = TryReadNumericLiteralPrefixWithRadix(token.Prefix);

        SingleInteger partFractional{};
        SingleInteger partExponent{};

        bool const hasPrefix = radix != 0;

        if (not hasPrefix)
        {
            radix = 10;
        }

        SingleInteger const partInteger = this->TryReadSingleInteger(token.Value, radix);
        if (hasPrefix and not partInteger.HasValue)
        {
            this->_diagnostic->AddError(this->_cursor.GetSpan(), "expected at least one digit in value");
        }

        if (this->_cursor.Peek() == U'.')
        {
            source::SourcePosition const start = this->_cursor.GetCurrentPosition();

            if (not partInteger.HasValue)
            {
                // Numbers cannot start from '.`.
                return false;
            }

            // Consume '.'
            this->_cursor.Advance();
            size_t const revertSize = token.Value.size();
            token.Value.push_back('.');

            partFractional = this->TryReadSingleInteger(token.Value, radix);

            bool revert = false;

            if (not partFractional.HasValue)
            {
                // Matched '[integer]['.']['_'+]' but without fractional part. Revert '.'
                revert = true;
            }
            else if (partFractional.HasLeadingSeparator)
            {
                // Fractional part cannot start with '_'.
                this->_diagnostic->AddError(this->_cursor.GetSpanToCurrent(start), "fractional part must not start with '_'");
                revert = true;
            }

            if (revert)
            {
                token.Value.resize(revertSize);
                this->_cursor.Reset(start);
            }
        }

        if (char32_t const exponent = this->_cursor.Peek(); CharTraits::IsDecimalExponent(exponent) or CharTraits::IsHexadecimalExponent(exponent))
        {
            this->_cursor.Advance();

            size_t const revertSize = token.Value.size();

            if (radix == 10)
            {
                if (not CharTraits::IsDecimalExponent(exponent))
                {
                    this->_diagnostic->AddError(this->_cursor.GetSpanForCurrent(), "invalid decimal exponent");
                }

                token.Value.push_back('e');
            }
            else if (radix == 16)
            {
                if (not CharTraits::IsHexadecimalExponent(exponent))
                {
                    this->_diagnostic->AddError(this->_cursor.GetSpanForCurrent(), "invalid hexadecimal exponent");
                }

                token.Value.push_back('p');
            }

            if (char32_t const sign = this->_cursor.Peek(); (sign == '+') or (sign == '-'))
            {
                this->_cursor.Advance();
                token.Value.push_back(static_cast<char>(sign));
            }

            partExponent = this->TryReadSingleInteger(token.Value, 10);

            if (not partExponent.HasValue)
            {
                // There is no integer part after exponent.
                this->_diagnostic->AddError(this->_cursor.GetSpan(), "expected at least one digit in exponent");

                token.Value.resize(revertSize);
            }
        }

        this->TryReadLiteralSuffix(token.Suffix);

        if (partFractional.HasValue or partExponent.HasValue)
        {
            token.Kind = SyntaxKind::FloatLiteralToken;

            if (radix == 2)
            {
                this->_diagnostic->AddError(this->_cursor.GetSpan(), "binary float literals are not supported");
            }
            else if (radix == 8)
            {
                this->_diagnostic->AddError(this->_cursor.GetSpan(), "octal float literals are not supported");
            }
        }
        else
        {
            token.Kind = SyntaxKind::IntegerLiteralToken;
        }

        if ((radix == 16) and partFractional.HasValue and (not partExponent.HasValue))
        {
            this->_diagnostic->AddError(this->_cursor.GetSpan(), "hexadecimal floating literal requires exponent");
        }

        if (token.Value.empty())
        {
            token.Value.push_back('0');
        }

        return true;
    }

    bool Lexer::TryReadPunctuation(TokenInfo& token)
    {
        switch (this->_cursor.Peek())
        {
        case U'~':
            {
                this->_cursor.Advance();
                token.Kind = SyntaxKind::TildeToken;
                return true;
            }

        case U'!':
            {
                this->_cursor.Advance();

                if (this->_cursor.First(U'='))
                {
                    token.Kind = SyntaxKind::ExclamationEqualsToken;
                }
                else if (this->_cursor.First(U'['))
                {
                    token.Kind = SyntaxKind::ExclamationOpenBracketToken;
                }
                else
                {
                    token.Kind = SyntaxKind::ExclamationToken;
                }

                return true;
            }

        case U'+':
            {
                this->_cursor.Advance();

                if (this->_cursor.First(U'='))
                {
                    token.Kind = SyntaxKind::PlusEqualsToken;
                }
                else if (this->_cursor.First(U'+'))
                {
                    token.Kind = SyntaxKind::PlusPlusToken;
                }
                else
                {
                    token.Kind = SyntaxKind::PlusToken;
                }

                return true;
            }

        case U'-':
            {
                this->_cursor.Advance();

                if (this->_cursor.First(U'='))
                {
                    token.Kind = SyntaxKind::MinusEqualsToken;
                }
                else if (this->_cursor.First(U'-'))
                {
                    token.Kind = SyntaxKind::MinusMinusToken;
                }
                else if (this->_cursor.First(U'>'))
                {
                    token.Kind = SyntaxKind::MinusGreaterThanToken;
                }
                else
                {
                    token.Kind = SyntaxKind::MinusToken;
                }

                return true;
            }

        case U'*':
            {
                this->_cursor.Advance();

                if (this->_cursor.First(U'='))
                {
                    token.Kind = SyntaxKind::AsteriskEqualsToken;
                }
                else
                {
                    token.Kind = SyntaxKind::AsteriskToken;
                }

                return true;
            }

        case U'/':
            {
                this->_cursor.Advance();

                if (this->_cursor.First(U'='))
                {
                    token.Kind = SyntaxKind::SlashEqualsToken;
                }
                else
                {
                    token.Kind = SyntaxKind::SlashToken;
                }

                return true;
            }

        case U'(':
            {
                this->_cursor.Advance();

                token.Kind = SyntaxKind::OpenParenToken;

                return true;
            }

        case U')':
            {
                this->_cursor.Advance();

                token.Kind = SyntaxKind::CloseParenToken;

                return true;
            }

        case U'{':
            {
                this->_cursor.Advance();

                token.Kind = SyntaxKind::OpenBraceToken;

                return true;
            }

        case U'}':
            {
                this->_cursor.Advance();

                token.Kind = SyntaxKind::CloseBraceToken;

                return true;
            }

        case U'[':
            {
                this->_cursor.Advance();

                token.Kind = SyntaxKind::OpenBracketToken;

                return true;
            }

        case U']':
            {
                this->_cursor.Advance();

                token.Kind = SyntaxKind::CloseBracketToken;

                return true;
            }

        case U':':
            {
                this->_cursor.Advance();

                if (this->_cursor.First(U':'))
                {
                    if (this->_cursor.First(U'<'))
                    {
                        token.Kind = SyntaxKind::ColonColonLessThanToken;
                    }
                    else if (this->_cursor.First(U'['))
                    {
                        token.Kind = SyntaxKind::ColonColonOpenBracketToken;
                    }
                    else
                    {
                        token.Kind = SyntaxKind::ColonColonToken;
                    }
                }
                else
                {
                    token.Kind = SyntaxKind::ColonToken;
                }

                return true;
            }

        case U';':
            {
                this->_cursor.Advance();

                token.Kind = SyntaxKind::SemicolonToken;

                return true;
            }

        case U'@':
            {
                this->_cursor.Advance();

                token.Kind = SyntaxKind::AtToken;

                return true;
            }

        case U'#':
            {
                this->_cursor.Advance();

                if (this->_cursor.First(U'['))
                {
                    token.Kind = SyntaxKind::HashOpenBracketToken;
                }
                else
                {
                    token.Kind = SyntaxKind::HashToken;
                }

                return true;
            }

        case U'$':
            {
                this->_cursor.Advance();

                token.Kind = SyntaxKind::DollarToken;

                return true;
            }

        case U'%':
            {
                this->_cursor.Advance();

                if (this->_cursor.First(U'='))
                {
                    token.Kind = SyntaxKind::PercentEqualsToken;
                }
                else
                {
                    token.Kind = SyntaxKind::PercentToken;
                }
                return true;
            }

        case U'^':
            {
                this->_cursor.Advance();

                if (this->_cursor.First(U'='))
                {
                    token.Kind = SyntaxKind::CaretEqualsToken;
                }
                else
                {
                    token.Kind = SyntaxKind::CaretToken;
                }

                return true;
            }

        case U'&':
            {
                this->_cursor.Advance();

                if (this->_cursor.First(U'&'))
                {
                    token.Kind = SyntaxKind::AmpersandAmpersandToken;
                }
                else if (this->_cursor.First(U'='))
                {
                    token.Kind = SyntaxKind::AmpersandEqualsToken;
                }
                else
                {
                    token.Kind = SyntaxKind::AmpersandToken;
                }

                return true;
            }

        case U'=':
            {
                this->_cursor.Advance();

                if (this->_cursor.First(U'='))
                {
                    token.Kind = SyntaxKind::EqualsEqualsToken;
                }
                else if (this->_cursor.First(U'>'))
                {
                    token.Kind = SyntaxKind::EqualsGreaterThanToken;
                }
                else
                {
                    token.Kind = SyntaxKind::EqualsToken;
                }

                return true;
            }

        case U'\\':
            {
                this->_cursor.Advance();

                token.Kind = SyntaxKind::BackslashToken;

                return true;
            }

        case U'|':
            {
                this->_cursor.Advance();

                if (this->_cursor.First(U'|'))
                {
                    token.Kind = SyntaxKind::BarBarToken;
                }
                else if (this->_cursor.First(U'='))
                {
                    token.Kind = SyntaxKind::BarEqualsToken;
                }
                else
                {
                    token.Kind = SyntaxKind::BarToken;
                }

                return true;
            }

        case U',':
            {
                this->_cursor.Advance();

                token.Kind = SyntaxKind::CommaToken;

                return true;
            }

        case U'<':
            {
                this->_cursor.Advance();

                if (this->_cursor.First(U'='))
                {
                    token.Kind = SyntaxKind::LessThanEqualsToken;
                }
                else if (this->_cursor.First(U'<'))
                {
                    if (this->_cursor.First(U'='))
                    {
                        token.Kind = SyntaxKind::LessThanLessThanEqualsToken;
                    }
                    else
                    {
                        token.Kind = SyntaxKind::LessThanLessThanToken;
                    }
                }
                else
                {
                    token.Kind = SyntaxKind::LessThanToken;
                }

                return true;
            }

        case U'>':
            {
                this->_cursor.Advance();

                if (this->_cursor.First(U'='))
                {
                    token.Kind = SyntaxKind::GreaterThanEqualsToken;
                }
                else if (this->_cursor.First(U'>'))
                {
                    if (this->_cursor.First(U'='))
                    {
                        token.Kind = SyntaxKind::GreaterThanGreaterThanEqualsToken;
                    }
                    else
                    {
                        token.Kind = SyntaxKind::GreaterThanGreaterThanToken;
                    }
                }
                else
                {
                    token.Kind = SyntaxKind::GreaterThanToken;
                }

                return true;
            }

        case U'?':
            {
                this->_cursor.Advance();

                if (this->_cursor.First(U'?'))
                {
                    if (this->_cursor.First(U'='))
                    {
                        token.Kind = SyntaxKind::QuestionQuestionEqualsToken;
                    }
                    else
                    {
                        token.Kind = SyntaxKind::QuestionQuestionToken;
                    }
                }
                else
                {
                    token.Kind = SyntaxKind::QuestionToken;
                }

                return true;
            }

        case U'.':
            {
                this->_cursor.Advance();

                if (this->_cursor.First(U'.'))
                {
                    if (this->_cursor.First(U'.'))
                    {
                        token.Kind = SyntaxKind::DotDotDotToken;
                    }
                    else
                    {
                        token.Kind = SyntaxKind::DotDotToken;
                    }
                }
                else
                {
                    token.Kind = SyntaxKind::DotToken;
                }

                return true;
            }

        default:
            {
                break;
            }
        }
        return false;
    }

    bool Lexer::TryReadIdentifier(TokenInfo& token)
    {
        if (CharTraits::IsIdentifierStart(this->_cursor.Peek()))
        {
            do
            {
                token.Value.append(this->_cursor.PeekAsStringView());
                this->_cursor.Advance();
            } while (CharTraits::IsIdentifierContinuation(this->_cursor.Peek()));

            return true;
        }

        return false;
    }

    bool Lexer::TryReadEndOfLine()
    {
        if (this->_cursor.First(U'\n'))
        {
            return true;
        }

        // TODO: Think about handling lone '\r' as well here.
        if (this->_cursor.Peek() == U'\r')
        {
            source::SourceCursor next = this->_cursor.NextCursor();

            if (next.First(U'\n'))
            {
                this->_cursor = next;
                return true;
            }
        }

        return false;
    }

    bool Lexer::TryReadWhitespace()
    {
        return this->_cursor.SkipIf(CharTraits::IsWhitespace);
    }

    SyntaxKind Lexer::TryReadSingleLineComment()
    {
        // Starts with '///' or '//!'
        bool const documentation = (this->_cursor.Peek() == U'/') or (this->_cursor.Peek() == U'!');

        while (true)
        {
            char32_t const current = this->_cursor.Peek();
            if (CharTraits::IsNewLine(current) or not this->_cursor.IsValid() or this->_cursor.IsEnd())
            {
                break;
            }

            this->_cursor.Advance();
        }

        return documentation
            ? SyntaxKind::SingleLineDocumentationTrivia
            : SyntaxKind::SingleLineCommentTrivia;
    }

    SyntaxKind Lexer::TryReadMultiLineComment()
    {
        // Starts with '/**' or '/*!'
        bool const documentation = (this->_cursor.Peek() == U'*') or (this->_cursor.Peek() == U'!');

        size_t depth = 1;

        while (true)
        {
            if (not this->_cursor.IsValid() or this->_cursor.IsEnd())
            {
                this->_diagnostic->AddError(
                    this->_cursor.GetSpan(),
                    "unterminated multi line comment");
                break;
            }

            if (this->_cursor.First(U'*'))
            {
                if (this->_cursor.First(U'/'))
                {
                    --depth;

                    if (depth == 0)
                    {
                        break;
                    }
                }
            }
            else if (this->_cursor.First(U'/'))
            {
                if (this->_cursor.First(U'*'))
                {
                    ++depth;
                }
            }
            else
            {
                this->_cursor.Advance();
            }
        }

        return documentation
            ? SyntaxKind::MultiLineDocumentationTrivia
            : SyntaxKind::MultiLineCommentTrivia;
    }

    SyntaxKind Lexer::TryReadComment()
    {
        source::SourcePosition const start = this->_cursor.GetCurrentPosition();

        if (this->_cursor.First(U'/'))
        {
            if (this->_cursor.First(U'/'))
            {
                return this->TryReadSingleLineComment();
            }

            if (this->_cursor.First(U'*'))
            {
                return this->TryReadMultiLineComment();
            }
        }

        this->_cursor.Reset(start);
        return SyntaxKind::None;
    }
}

namespace weave::syntax
{
    LiteralPrefixKind Lexer::TryReadStringPrefix()
    {
        if (this->_cursor.First(U'u'))
        {
            if (this->_cursor.First(U'1'))
            {
                if (this->_cursor.First(U'6'))
                {
                    return LiteralPrefixKind::Utf16;
                }
            }
            else if (this->_cursor.First(U'3'))
            {
                if (this->_cursor.First(U'2'))
                {
                    return LiteralPrefixKind::Utf32;
                }
            }
            else if (this->_cursor.First(U'8'))
            {
                return LiteralPrefixKind::Utf8;
            }
        }

        return LiteralPrefixKind::Default;
    }

    Lexer::SingleInteger Lexer::TryReadSingleInteger(std::string& builder, int base)
    {
        source::SourcePosition const start = this->_cursor.GetCurrentPosition();

        auto const charset = [base]
        {
            switch (base)
            {
            case 2:
                return CharTraits::IsBinaryDigit;

            case 8:
                return CharTraits::IsOctalDigit;

            case 16:
                return CharTraits::IsHexadecimalDigit;

            default:
                break;
            }

            return CharTraits::IsDecimalDigit;
        }();

        SingleInteger result{};

        if (this->_cursor.Peek() == U'_')
        {
            result.HasLeadingSeparator = true;
        }

        while (true)
        {
            char32_t const current = this->_cursor.Peek();

            if (current == U'_')
            {
                result.HasSeparator = true;
                result.HasTrailingSeparator = true;
            }
            else
            {
                if (not charset(current))
                {
                    if (CharTraits::IsDecimalDigit(current))
                    {
                        // Wrong digit for base.
                        this->_diagnostic->AddError(
                            this->_cursor.GetSpanForCurrent(),
                            fmt::format("invalid digit for base {} literal", base));
                    }
                    else
                    {
                        break;
                    }
                }

                builder.push_back(static_cast<char>(current));
                result.HasValue = true;
                result.HasTrailingSeparator = false;
            }

            this->_cursor.Advance();
        }

        if (not result.HasValue)
        {
            // Revert scanning, it was either empty match or more than one separators.
            this->_cursor.Reset(start);
        }

        return result;
    }

    size_t Lexer::TryReadDecNumber(uint64_t& result)
    {
        size_t count{};

        while (true)
        {
            if (not impl::AppendDecCharacter(result, this->_cursor.Peek()))
            {
                break;
            }

            this->_cursor.Advance();
            ++count;
        }

        return count;
    }

    size_t Lexer::TryReadHexNumber(uint64_t& result)
    {
        size_t count{};

        while (true)
        {
            if (not impl::AppendHexCharacter(result, this->_cursor.Peek()))
            {
                break;
            }

            this->_cursor.Advance();
            ++count;
        }

        return count;
    }

    size_t Lexer::TryReadDecNumber(uint64_t& result, size_t maxLength)
    {
        size_t count{};

        for (; count < maxLength; ++count)
        {
            if (not impl::AppendDecCharacter(result, this->_cursor.Peek()))
            {
                break;
            }

            this->_cursor.Advance();
        }

        return count;
    }

    size_t Lexer::TryReadHexNumber(uint64_t& result, size_t maxLength)
    {
        size_t count{};

        for (; count < maxLength; ++count)
        {
            if (not impl::AppendHexCharacter(result, this->_cursor.Peek()))
            {
                break;
            }

            this->_cursor.Advance();
        }

        return count;
    }

    bool Lexer::TryReadHexEscapeSequence(std::string& builder)
    {
        source::SourcePosition const start = this->_cursor.GetCurrentPosition();

        if (this->_cursor.First(U'x'))
        {
            uint64_t value{};

            size_t const count = this->TryReadHexNumber(value, 2);

            if (count != 2)
            {
                this->_diagnostic->AddError(this->_cursor.GetSpanToCurrent(start), "numeric character sequence is too short");

                // Emit invalid sequence later
                this->_cursor.Reset(start);
                return false;
            }

            if (value >= 0x80)
            {
                this->_diagnostic->AddError(
                    this->_cursor.GetSpanToCurrent(start),
                    R"(must be a character in the range[\x00-\x7F])");

                // Emit invalid sequence later
                this->_cursor.Reset(start);
                return false;
            }

            builder.push_back(static_cast<char>(value));

            return true;
        }

        return false;
    }

    bool Lexer::TryAppendUnicodeCodepoint(std::string& builder, char32_t c)
    {
        std::array<char, 8> buffer; // NOLINT(cppcoreguidelines-pro-type-member-init, hicpp-member-init)

        if (char* it = buffer.data(); unicode::Encode(it, it + buffer.size(), c) == unicode::ConversionResult::Success)
        {
            builder.append(buffer.data(), it);
            return true;
        }

        return false;
    }

    bool Lexer::TryReadTrivialEscapeSequence(std::string& builder)
    {
        int const matched = [c = this->_cursor.Peek()]() -> int
        {
            switch (c)
            {
            case U'\\':
                return '\\';

            case U'\'':
                return '\'';

            case U'"':
                return '"';

            case U'?':
                return '?';

            case U'0':
                return '\0';

            case U'a':
                return '\a';

            case U'b':
                return '\b';

            case U'e':
                return 0x1B;

            case U'f':
                return '\f';

            case U'n':
                return '\n';

            case U'r':
                return '\r';

            case U't':
                return '\t';

            case U'v':
                return '\v';

            default:
                break;
            }

            return -1;
        }();

        if (matched >= 0)
        {
            this->_cursor.Advance();
            builder.push_back(static_cast<char>(matched));
            return true;
        }

        this->_diagnostic->AddError(
            this->_cursor.GetSpanForCurrent(),
            "invalid character escape sequence");
        return false;
    }

    bool Lexer::TryReadUnicodeEscapeSequence(std::string& builder)
    {
        source::SourcePosition const start = this->_cursor.GetCurrentPosition();

        if (this->_cursor.First(U'u'))
        {
            if (this->_cursor.First(U'{'))
            {
                uint64_t value{};

                size_t const count = this->TryReadHexNumber(value, 6);

                if (count == 0)
                {
                    this->_diagnostic->AddError(
                        this->_cursor.GetSpanToCurrent(start),
                        "escape sequence must have at least 1 hex digit");
                }
                else if (count > 6)
                {
                    this->_diagnostic->AddError(
                        this->_cursor.GetSpanToCurrent(start),
                        "must have at most 6 hex digits");
                }

                if (this->_cursor.First(U'}'))
                {
                    if (not this->TryAppendUnicodeCodepoint(builder, static_cast<char32_t>(value)))
                    {
                        this->_diagnostic->AddError(
                            this->_cursor.GetSpanToCurrent(start),
                            "invalid unicode character escape sequence");
                    }

                    return true;
                }

                this->_diagnostic->AddError(
                    this->_cursor.GetSpanForCurrent(),
                    "missing closing '}' on unicode sequence");
            }
            else
            {
                this->_diagnostic->AddError(
                    this->_cursor.GetSpanToCurrent(start),
                    R"(expected '{' after '\\u')");
            }
        }

        this->_cursor.Reset(start);
        return false;
    }

    bool Lexer::TryReadEscapeSequence(std::string& builder)
    {
        if (this->TryReadHexEscapeSequence(builder))
        {
            return true;
        }

        if (this->TryReadUnicodeEscapeSequence(builder))
        {
            return true;
        }

        if (this->TryReadTrivialEscapeSequence(builder))
        {
            return true;
        }

        return false;
    }

    bool Lexer::TryReadStringOrCharacterLiteralCore(std::string& builder, char32_t specifier, size_t& consumed)
    {
        if (not this->_cursor.First(specifier))
        {
            return false;
        }

        bool terminated = false;
        bool escaping = false;

        while (true)
        {
            if (not this->_cursor.IsValid() or this->_cursor.IsEnd())
            {
                break;
            }

            if (escaping)
            {
                escaping = false;

                if (not this->TryReadEscapeSequence(builder))
                {
                    // All we can do is to append escaped character as-is.
                    builder.append(this->_cursor.PeekAsStringView());
                    this->_cursor.Advance();
                }

                ++consumed;
            }
            else
            {
                if (this->_cursor.First(specifier))
                {
                    terminated = true;
                    break;
                }

                if (this->TryReadEndOfLine())
                {
                    break;
                }

                if (this->_cursor.First(U'\\'))
                {
                    escaping = true;
                }
                else
                {
                    // Append whatever we decoded.
                    builder.append(this->_cursor.PeekAsStringView());
                    this->_cursor.Advance();

                    ++consumed;
                }
            }
        }

        if (not terminated)
        {
            this->_diagnostic->AddError(this->_cursor.GetSpan(), "unterminated string literal");
        }

        if (escaping)
        {
            this->_diagnostic->AddError(this->_cursor.GetSpan(), "unterminated escape sequence");
        }

        return terminated;
    }

    int Lexer::TryReadNumericLiteralPrefixWithRadix(LiteralPrefixKind& prefix)
    {
        source::SourcePosition const start = this->_cursor.GetCurrentPosition();

        if (this->_cursor.First(U'0'))
        {
            switch (this->_cursor.Peek())
            {
            case U'B':
                this->_diagnostic->AddError(this->_cursor.GetSpanToNext(start), "invalid base prefix for binary number literal");
                [[fallthrough]];
            case U'b':
                prefix = LiteralPrefixKind::Binary;
                this->_cursor.Advance();
                return 2;

            case U'O':
                this->_diagnostic->AddError(this->_cursor.GetSpanToNext(start), "invalid base prefix for octal number literal");
                [[fallthrough]];
            case U'o':
                prefix = LiteralPrefixKind::Octal;
                this->_cursor.Advance();
                return 8;

            case U'X':
                this->_diagnostic->AddError(this->_cursor.GetSpanToNext(start), "invalid base prefix for hexadecimal number literal");
                [[fallthrough]];
            case U'x':
                prefix = LiteralPrefixKind::Hexadecimal;
                this->_cursor.Advance();
                return 16;

            default:
                this->_cursor.Reset(start);
                return 0;
            }
        }

        return 10;
    }

    void Lexer::TryReadLiteralSuffix(std::string& builder)
    {
        if (CharTraits::IsIdentifierStart(this->_cursor.Peek()))
        {
            source::SourcePosition const start = this->_cursor.GetCurrentPosition();

            do
            {
                this->_cursor.Advance();
            } while (CharTraits::IsIdentifierContinuation(this->_cursor.Peek()));

            source::SourceSpan const suffixSpan = this->_cursor.GetSpanToCurrent(start);

            std::string_view const view = this->_source->GetText(suffixSpan);

            builder.assign(view);
        }
    }
}
