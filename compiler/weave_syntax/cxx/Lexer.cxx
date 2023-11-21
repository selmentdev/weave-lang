#include "weave/syntax/Lexer.hxx"
#include "weave/Unicode.hxx"
#include "weave/syntax/CharTraits.hxx"

#include <array>

namespace weave::syntax
{
    bool Lexer::Lex()
    {
        this->TryReadTrivia(this->_token_leading_trivia, true);

        bool const lexed = this->TryReadToken();

        this->TryReadTrivia(this->_token_trailing_trivia, false);

        return lexed;
    }

    bool Lexer::TryReadToken()
    {
        this->_cursor.Start();
        this->_token_value.clear();
        this->_token_prefix.clear();
        this->_token_suffix.clear();

        if (this->_cursor.IsEnd())
        {
            this->_token_kind = TokenKind::EndOfFile;
            this->_token_span = this->_cursor.GetSpan();
            return true;
        }

        if (this->TryReadRawIdentifier())
        {
            this->_token_kind = TokenKind::Identifier;
            this->_token_span = this->_cursor.GetSpan();
            return true;
        }

        if (this->TryReadStringLiteral())
        {
            this->_token_kind = TokenKind::StringLiteral;
            this->_token_span = this->_cursor.GetSpan();
            return true;
        }

        if (this->TryReadCharacterLiteral())
        {
            this->_token_kind = TokenKind::CharacterLiteral;
            this->_token_span = this->_cursor.GetSpan();
            return true;
        }

        if (this->TryReadNumericLiteral())
        {
            this->_token_span = this->_cursor.GetSpan();
            return true;
        }

        if (this->TryReadPunctuation())
        {
            this->_token_span = this->_cursor.GetSpan();
            return true;
        }

        if (this->TryReadIdentifier())
        {
            if (std::optional<TokenKind> const keyword = TokenKindTraits::TryMapIdentifierToKeyword(this->_token_value); keyword.has_value())
            {
                this->_token_kind = *keyword;
            }
            else
            {
                this->_token_kind = TokenKind::Identifier;
            }

            this->_token_span = this->_cursor.GetSpan();
            return true;
        }

        if (this->_cursor.IsValid())
        {
            // Current character is not recognized.
            this->_diagnostic.AddError(this->_cursor.GetSpan(), "unexpected character");
        }
        else
        {
            // Character was not encoded in UTF8. This is not a EOF.
            this->_diagnostic.AddError(this->_cursor.GetSpan(), "invalid UTF-8 character");
        }


        this->_token_kind = TokenKind::Error;
        this->_token_span = this->_cursor.GetSpan();
        return false;
    }

    void Lexer::TryReadTrivia(std::vector<Trivia>& builder, bool leading)
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
                if (this->_triviaMode == TriviaMode::All)
                {
                    builder.emplace_back(TriviaKind::EndOfLine, this->_cursor.GetSpan());
                }

                if (not leading)
                {
                    break;
                }
            }
            else if (this->TryReadWhitespace())
            {
                if (this->_triviaMode == TriviaMode::All)
                {
                    builder.emplace_back(TriviaKind::Whitespace, this->_cursor.GetSpan());
                }
            }
            else if (TriviaKind kind = this->TryReadComment(); kind != TriviaKind::Error)
            {
                if ((this->_triviaMode == TriviaMode::All) or ((this->_triviaMode == TriviaMode::Documentation) and (TriviaKindTraits::IsDocumentation(kind))))
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

    bool Lexer::TryReadRawIdentifier()
    {
        source::SourcePosition const start = this->_cursor.GetCurrentPosition();

        if (this->_cursor.First(U'r'))
        {
            if (this->_cursor.First(U'#'))
            {
                if (this->TryReadIdentifier())
                {
                    return true;
                }
            }
        }

        this->_cursor.Reset(start);
        return false;
    }

    bool Lexer::TryReadRawStringLiteral()
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
                            this->_diagnostic.AddError(
                                this->_cursor.GetSpanToCurrent(startTerminator),
                                "raw string literal terminator too long");

                            terminated = true;
                            break;
                        }

                        // Append unmatched characters.
                        this->_token_value.push_back('"');
                        this->_token_value.append(matched, '#');
                    }
                    else
                    {
                        // Append whatever we decoded.
                        this->_token_value.append(this->_cursor.PeekAsStringView());
                        this->_cursor.Advance();
                    }
                }

                if (not terminated)
                {
                    this->_diagnostic.AddError(
                        this->_cursor.GetSpan(),
                        "missing terminating character error");
                }

                return terminated;
            }
        }

        this->_cursor.Reset(start);
        return false;
    }

    bool Lexer::TryReadDefaultStringLiteral()
    {
        size_t consumed{};
        if (this->TryReadStringOrCharacterLiteralCore(U'"', consumed))
        {
            return true;
        }

        return false;
    }

    bool Lexer::TryReadStringPrefix()
    {
        if (this->_cursor.First(U'u'))
        {
            if (this->_cursor.First(U'1'))
            {
                if (this->_cursor.First(U'6'))
                {
                    this->_token_prefix.assign("u16");
                    return true;
                }
            }
            else if (this->_cursor.First(U'3'))
            {
                if (this->_cursor.First(U'2'))
                {
                    this->_token_prefix.assign("u32");
                    return true;
                }
            }
            else if (this->_cursor.First(U'8'))
            {
                // u8
                this->_token_prefix.assign("u8");
                return true;
            }
        }

        return false;
    }

    bool Lexer::TryReadStringLiteral()
    {
        source::SourcePosition const start = this->_cursor.GetCurrentPosition();

        if (!this->TryReadStringPrefix())
        {
            // Not a string prefix. Continue trying as string literal.
            this->_cursor.Reset(start);
        }
        
        if (this->TryReadRawStringLiteral())
        {
            return true;
        }

        if (this->TryReadDefaultStringLiteral())
        {
            return true;
        }

        // Not a string literal.
        this->_cursor.Reset(start);
        return false;
    }

    bool Lexer::TryReadCharacterLiteral()
    {
        size_t consumed{};

        if (this->TryReadStringOrCharacterLiteralCore(U'\'', consumed))
        {
            if (consumed == 0)
            {
                this->_diagnostic.AddError(this->_cursor.GetSpan(), "empty character literal");
            }
            else if (consumed > 1)
            {
                this->_diagnostic.AddError(this->_cursor.GetSpan(), "character literal may only contain one codepoint");
            }

            return true;
        }

        return false;
    }

    bool Lexer::TryReadNumericLiteral()
    {
        if (not CharTraits::IsDecimalDigit(this->_cursor.Peek()))
        {
            return false;
        }

        int radix = TryReadNumberLiteralPrefixWithRadix();

        SingleInteger partFractional{};
        SingleInteger partExponent{};

        bool const hasPrefix = radix != 0;

        if (not hasPrefix)
        {
            radix = 10;
        }

        SingleInteger const partInteger = this->TryReadSingleInteger(radix);
        if (hasPrefix and not partInteger.HasValue)
        {
            this->_diagnostic.AddError(this->_cursor.GetSpan(), "expected at least one digit in value");
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
            size_t const revertSize = this->_token_value.size();
            this->_token_value.push_back('.');

            partFractional = this->TryReadSingleInteger(radix);

            bool revert = false;

            if (not partFractional.HasValue)
            {
                // Matched '[integer]['.']['_'+]' but without fractional part. Revert '.'
                revert = true;
            }
            else if (partFractional.HasLeadingSeparator)
            {
                // Fractional part cannot start with '_'.
                this->_diagnostic.AddError(this->_cursor.GetSpanToCurrent(start), "fractional part must not start with '_'");
                revert = true;
            }

            if (revert)
            {
                this->_token_value.resize(revertSize);
                this->_cursor.Reset(start);
            }
        }

        if (char32_t const exponent = this->_cursor.Peek(); CharTraits::IsDecimalExponent(exponent) or CharTraits::IsHexadecimalExponent(exponent))
        {
            this->_cursor.Advance();

            size_t const revertSize = this->_token_value.size();

            if (radix == 10)
            {
                if (not CharTraits::IsDecimalExponent(exponent))
                {
                    this->_diagnostic.AddError(this->_cursor.GetSpanForCurrent(), "invalid decimal exponent");
                }

                this->_token_value.push_back('e');
            }
            else if (radix == 16)
            {
                if (not CharTraits::IsHexadecimalExponent(exponent))
                {
                    this->_diagnostic.AddError(this->_cursor.GetSpanForCurrent(), "invalid hexadecimal exponent");
                }

                this->_token_value.push_back('p');
            }

            if (char32_t const sign = this->_cursor.Peek(); (sign == '+') or (sign == '-'))
            {
                this->_cursor.Advance();
                this->_token_value.push_back(static_cast<char>(sign));
            }

            partExponent = this->TryReadSingleInteger(10);

            if (not partExponent.HasValue)
            {
                // There is no integer part after exponent.
                this->_diagnostic.AddError(this->_cursor.GetSpan(), "expected at least one digit in exponent");

                this->_token_value.resize(revertSize);
            }
        }

        this->TryReadNumberValueType();

        if (partFractional.HasValue or partExponent.HasValue)
        {
            this->_token_kind = TokenKind::FloatLiteral;

            if (radix == 2)
            {
                this->_diagnostic.AddError(this->_cursor.GetSpan(), "binary float literals are not supported");
            }
            else if (radix == 8)
            {
                this->_diagnostic.AddError(this->_cursor.GetSpan(), "octal float literals are not supported");
            }
        }
        else
        {
            this->_token_kind = TokenKind::IntegerLiteral;
        }

        if ((radix == 16) and partFractional.HasValue and (not partExponent.HasValue))
        {
            this->_diagnostic.AddError(this->_cursor.GetSpan(), "hexadecimal floating literal requires exponent");
        }

        return true;
    }

    bool Lexer::TryReadPunctuation()
    {
        switch (this->_cursor.Peek())
        {
        case U'~':
            {
                this->_cursor.Advance();
                this->_token_kind = TokenKind::TildeToken;
                return true;
            }

        case U'!':
            {
                this->_cursor.Advance();

                if (this->_cursor.First(U'='))
                {
                    this->_token_kind = TokenKind::ExclamationEqualsToken;
                }
                else
                {
                    this->_token_kind = TokenKind::ExclamationToken;
                }

                return true;
            }

        case U'+':
            {
                this->_cursor.Advance();

                if (this->_cursor.First(U'='))
                {
                    this->_token_kind = TokenKind::PlusEqualsToken;
                }
                else if (this->_cursor.First(U'+'))
                {
                    this->_token_kind = TokenKind::PlusPlusToken;
                }
                else
                {
                    this->_token_kind = TokenKind::PlusToken;
                }

                return true;
            }

        case U'-':
            {
                this->_cursor.Advance();

                if (this->_cursor.First(U'='))
                {
                    this->_token_kind = TokenKind::MinusEqualsToken;
                }
                else if (this->_cursor.First(U'-'))
                {
                    this->_token_kind = TokenKind::MinusMinusToken;
                }
                else if (this->_cursor.First(U'>'))
                {
                    this->_token_kind = TokenKind::MinusGreaterThanToken;
                }
                else
                {
                    this->_token_kind = TokenKind::MinusToken;
                }

                return true;
            }

        case U'*':
            {
                this->_cursor.Advance();

                if (this->_cursor.First(U'='))
                {
                    this->_token_kind = TokenKind::AsteriskEqualsToken;
                }
                else
                {
                    this->_token_kind = TokenKind::AsteriskToken;
                }

                return true;
            }

        case U'/':
            {
                this->_cursor.Advance();

                if (this->_cursor.First(U'='))
                {
                    this->_token_kind = TokenKind::SlashEqualsToken;
                }
                else
                {
                    this->_token_kind = TokenKind::SlashToken;
                }

                return true;
            }

        case U'(':
            {
                this->_cursor.Advance();

                this->_token_kind = TokenKind::OpenParenToken;

                return true;
            }

        case U')':
            {
                this->_cursor.Advance();

                this->_token_kind = TokenKind::CloseParenToken;

                return true;
            }

        case U'{':
            {
                this->_cursor.Advance();

                this->_token_kind = TokenKind::OpenBraceToken;

                return true;
            }

        case U'}':
            {
                this->_cursor.Advance();

                this->_token_kind = TokenKind::CloseBraceToken;

                return true;
            }

        case U'[':
            {
                this->_cursor.Advance();

                this->_token_kind = TokenKind::OpenBracketToken;

                return true;
            }

        case U']':
            {
                this->_cursor.Advance();

                this->_token_kind = TokenKind::CloseBracketToken;

                return true;
            }

        case U':':
            {
                this->_cursor.Advance();

                if (this->_cursor.First(U':'))
                {
                    this->_token_kind = TokenKind::ColonColonToken;
                }
                else
                {
                    this->_token_kind = TokenKind::ColonToken;
                }

                return true;
            }

        case U';':
            {
                this->_cursor.Advance();

                this->_token_kind = TokenKind::SemicolonToken;

                return true;
            }

        case U'@':
            {
                this->_cursor.Advance();

                this->_token_kind = TokenKind::AtToken;

                return true;
            }

        case U'#':
            {
                this->_cursor.Advance();

                this->_token_kind = TokenKind::HashToken;

                return true;
            }

        case U'$':
            {
                this->_cursor.Advance();

                this->_token_kind = TokenKind::DollarToken;

                return true;
            }

        case U'%':
            {
                this->_cursor.Advance();

                if (this->_cursor.First(U'='))
                {
                    this->_token_kind = TokenKind::PercentEqualsToken;
                }
                else
                {
                    this->_token_kind = TokenKind::PercentToken;
                }
                return true;
            }

        case U'^':
            {
                this->_cursor.Advance();

                if (this->_cursor.First(U'^'))
                {
                    this->_token_kind = TokenKind::CaretEqualsToken;
                }
                else
                {
                    this->_token_kind = TokenKind::CaretToken;
                }

                return true;
            }

        case U'&':
            {
                this->_cursor.Advance();

                if (this->_cursor.First(U'&'))
                {
                    this->_token_kind = TokenKind::AmpersandAmpersandToken;
                }
                else if (this->_cursor.First(U'='))
                {
                    this->_token_kind = TokenKind::AmpersandEqualsToken;
                }
                else
                {
                    this->_token_kind = TokenKind::AmpersandToken;
                }

                return true;
            }

        case U'=':
            {
                this->_cursor.Advance();

                if (this->_cursor.First(U'='))
                {
                    this->_token_kind = TokenKind::EqualsEqualsToken;
                }
                else if (this->_cursor.First(U'>'))
                {
                    this->_token_kind = TokenKind::EqualsGreaterThanToken;
                }
                else
                {
                    this->_token_kind = TokenKind::EqualsToken;
                }

                return true;
            }

        case U'\\':
            {
                this->_cursor.Advance();

                this->_token_kind = TokenKind::BackslashToken;

                return true;
            }

        case U'|':
            {
                this->_cursor.Advance();

                if (this->_cursor.First(U'|'))
                {
                    this->_token_kind = TokenKind::BarBarToken;
                }
                else if (this->_cursor.First(U'='))
                {
                    this->_token_kind = TokenKind::BarEqualsToken;
                }
                else
                {
                    this->_token_kind = TokenKind::BarToken;
                }

                return true;
            }

        case U',':
            {
                this->_cursor.Advance();

                this->_token_kind = TokenKind::CommaToken;

                return true;
            }

        case U'<':
            {
                this->_cursor.Advance();

                if (this->_cursor.First(U'='))
                {
                    this->_token_kind = TokenKind::LessThanEqualsToken;
                }
                else if (this->_cursor.First(U'<'))
                {
                    if (this->_cursor.First(U'='))
                    {
                        this->_token_kind = TokenKind::LessThanLessThanEqualsToken;
                    }
                    else
                    {
                        this->_token_kind = TokenKind::LessThanLessThanToken;
                    }
                }
                else
                {
                    this->_token_kind = TokenKind::LessThanToken;
                }

                return true;
            }

        case U'>':
            {
                this->_cursor.Advance();

                if (this->_cursor.First(U'='))
                {
                    this->_token_kind = TokenKind::GreaterThanEqualsToken;
                }
                else if (this->_cursor.First(U'>'))
                {
                    if (this->_cursor.First(U'='))
                    {
                        this->_token_kind = TokenKind::GreaterThanGreaterThanEqualsToken;
                    }
                    else
                    {
                        this->_token_kind = TokenKind::GreaterThanGreaterThanToken;
                    }
                }
                else
                {
                    this->_token_kind = TokenKind::GreaterThanToken;
                }

                return true;
            }

        case U'?':
            {
                this->_cursor.Advance();

                if (this->_cursor.First(U'?'))
                {
                    this->_token_kind = TokenKind::QuestionQuestionToken;
                }
                else
                {
                    this->_token_kind = TokenKind::QuestionToken;
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
                        this->_token_kind = TokenKind::DotDotDotToken;
                    }
                    else
                    {
                        this->_token_kind = TokenKind::DotDotToken;
                    }
                }
                else
                {
                    this->_token_kind = TokenKind::DotToken;
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

    TriviaKind Lexer::TryReadSingleLineComment()
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
            ? TriviaKind::SingleLineDocumentation
            : TriviaKind::SingleLineComment;
    }

    TriviaKind Lexer::TryReadMultiLineComment()
    {
        // Starts with '/**' or '/*!'
        bool const documentation = (this->_cursor.Peek() == U'*') or (this->_cursor.Peek() == U'!');

        size_t depth = 1;

        while (true)
        {
            if (not this->_cursor.IsValid() or this->_cursor.IsEnd())
            {
                this->_diagnostic.AddError(
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
            ? TriviaKind::MultiLineDocumentation
            : TriviaKind::MultiLineComment;
    }

    TriviaKind Lexer::TryReadComment()
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
        return TriviaKind::Error;
    }

    bool Lexer::TryReadIdentifier()
    {
        if (CharTraits::IsIdentifierStart(this->_cursor.Peek()))
        {
            do
            {
                this->_token_value.append(this->_cursor.PeekAsStringView());
                this->_cursor.Advance();
            } while (CharTraits::IsIdentifierContinuation(this->_cursor.Peek()));

            return true;
        }

        return false;
    }

    Lexer::SingleInteger Lexer::TryReadSingleInteger(int base)
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
                        this->_diagnostic.AddError(
                            this->_cursor.GetSpanForCurrent(),
                            fmt::format("invalid digit for base {} literal", base));
                    }
                    else
                    {
                        break;
                    }
                }

                this->_token_value.push_back(static_cast<char>(current));
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
            if (not AppendDecCharacter(result, this->_cursor.Peek()))
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
            if (not AppendHexCharacter(result, this->_cursor.Peek()))
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
            if (not AppendDecCharacter(result, this->_cursor.Peek()))
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
            if (not AppendHexCharacter(result, this->_cursor.Peek()))
            {
                break;
            }

            this->_cursor.Advance();
        }

        return count;
    }

    bool Lexer::TryReadHexEscapeSequence()
    {
        source::SourcePosition const start = this->_cursor.GetCurrentPosition();

        if (this->_cursor.First(U'x'))
        {
            uint64_t value{};

            size_t const count = this->TryReadHexNumber(value, 2);

            if (count != 2)
            {
                this->_diagnostic.AddError(this->_cursor.GetSpanToCurrent(start), "numeric character sequence is too short");

                // Emit invalid sequence later
                this->_cursor.Reset(start);
                return false;
            }

            if (value >= 0x80)
            {
                this->_diagnostic.AddError(
                    this->_cursor.GetSpanToCurrent(start),
                    R"(must be a character in the range[\x00-\x7F])");

                // Emit invalid sequence later
                this->_cursor.Reset(start);
                return false;
            }

            this->_token_value.push_back(static_cast<char>(value));

            return true;
        }

        return false;
    }

    bool Lexer::TryAppendUnicodeCodepoint(char32_t c)
    {
        std::array<char, 8> buffer; // NOLINT(cppcoreguidelines-pro-type-member-init, hicpp-member-init)

        if (char* it = buffer.data(); unicode::Encode(it, it + buffer.size(), c) == unicode::ConversionResult::Success)
        {
            this->_token_value.append(buffer.data(), it);
            return true;
        }

        return false;
    }

    bool Lexer::TryReadTrivialEscapeSequence()
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
            this->_token_value.push_back(static_cast<char>(matched));
            return true;
        }

        this->_diagnostic.AddError(
            this->_cursor.GetSpanForCurrent(),
            "invalid character escape sequence");
        return false;
    }

    bool Lexer::TryReadUnicodeEscapeSequence()
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
                    this->_diagnostic.AddError(
                        this->_cursor.GetSpanToCurrent(start),
                        "escape sequence must have at least 1 hex digit");
                }
                else if (count > 6)
                {
                    this->_diagnostic.AddError(
                        this->_cursor.GetSpanToCurrent(start),
                        "must have at most 6 hex digits");
                }

                if (this->_cursor.First(U'}'))
                {
                    if (not this->TryAppendUnicodeCodepoint(static_cast<char32_t>(value)))
                    {
                        this->_diagnostic.AddError(
                            this->_cursor.GetSpanToCurrent(start),
                            "invalid unicode character escape sequence");
                    }

                    return true;
                }

                this->_diagnostic.AddError(
                    this->_cursor.GetSpanForCurrent(),
                    "missing closing '}' on unicode sequence");
            }
            else
            {
                this->_diagnostic.AddError(
                    this->_cursor.GetSpanToCurrent(start),
                    R"(expected '{' after '\\u')");
            }
        }

        this->_cursor.Reset(start);
        return false;
    }

    bool Lexer::TryReadEscapeSequence()
    {
        if (this->TryReadHexEscapeSequence())
        {
            return true;
        }

        if (this->TryReadUnicodeEscapeSequence())
        {
            return true;
        }

        if (this->TryReadTrivialEscapeSequence())
        {
            return true;
        }

        return false;
    }

    bool Lexer::TryReadStringOrCharacterLiteralCore(char32_t specifier, size_t& consumed)
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

                if (not this->TryReadEscapeSequence())
                {
                    // All we can do is to append escaped character as-is.
                    this->_token_value.append(this->_cursor.PeekAsStringView());
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
                    this->_token_value.append(this->_cursor.PeekAsStringView());
                    this->_cursor.Advance();

                    ++consumed;
                }
            }
        }

        if (not terminated)
        {
            this->_diagnostic.AddError(this->_cursor.GetSpan(), "unterminated string literal");
        }

        if (escaping)
        {
            this->_diagnostic.AddError(this->_cursor.GetSpan(), "unterminated escape sequence");
        }

        return terminated;
    }

    int Lexer::TryReadNumberLiteralPrefixWithRadix()
    {
        source::SourcePosition const start = this->_cursor.GetCurrentPosition();

        if (this->_cursor.First(U'0'))
        {
            switch (this->_cursor.Peek())
            {
            case U'B':
                this->_diagnostic.AddError(this->_cursor.GetSpanToNext(start), "invalid base prefix for binary number literal");
                [[fallthrough]];
            case U'b':
                this->_token_prefix.assign("0b");
                this->_cursor.Advance();
                return 2;

            case U'O':
                this->_diagnostic.AddError(this->_cursor.GetSpanToNext(start), "invalid base prefix for octal number literal");
                [[fallthrough]];
            case U'o':
                this->_token_prefix.assign("0o");
                this->_cursor.Advance();
                return 8;

            case U'X':
                this->_diagnostic.AddError(this->_cursor.GetSpanToNext(start), "invalid base prefix for hexadecimal number literal");
                [[fallthrough]];
            case U'x':
                this->_token_prefix.assign("0x");
                this->_cursor.Advance();
                return 16;

            default:
                this->_cursor.Reset(start);
                return 0;
            }
        }

        return 10;
    }

    void Lexer::TryReadNumberValueType()
    {
        source::SourcePosition const start = this->_cursor.GetCurrentPosition();

        if (CharTraits::IsIdentifierStart(this->_cursor.Peek()))
        {
            do
            {
                this->_cursor.Advance();
            } while (CharTraits::IsIdentifierContinuation(this->_cursor.Peek()));

            source::SourceSpan const suffixSpan = this->_cursor.GetSpanToCurrent(start);

            std::string_view const suffix = this->_source.GetText(suffixSpan);

            this->_token_suffix.assign(suffix);
        }
    }
}
