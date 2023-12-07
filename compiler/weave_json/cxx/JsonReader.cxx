#include "weave/json/JsonReader.hxx"
#include "weave/Unicode.hxx"
#include "weave/bugcheck/Assert.hxx"


// Json Grammar
namespace weave::json::impl
{
    [[nodiscard]] constexpr size_t ConsumeWhitespace(std::string_view source) noexcept
    {
        //  ws
        //      ""
        //      '0020' ws
        //      '000A' ws
        //      '000D' ws
        //      '0009' ws

        size_t const count = source.size();
        size_t consumed = 0;

        for (; consumed < count; ++consumed)
        {
            switch (source[consumed])
            {
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                break;

            default:
                return consumed;
            }
        }

        return consumed;
    }

    [[nodiscard]] constexpr size_t ConsumeLineComment(std::string_view source) noexcept
    {
        size_t const count = source.size();
        size_t consumed = 0;

        if ((count >= 2) and (source[1] == '/'))
        {
            for (consumed += 2; consumed < count; ++consumed)
            {
                char const current = source[consumed];

                if ((current == '\n') or (current == '\r'))
                {
                    break;
                }
            }
        }

        return consumed;
    }

    [[nodiscard]] constexpr size_t ConsumeBlockComment(std::string_view source) noexcept
    {
        size_t const count = source.size();

        // Requires to parse at least '/**/' characters
        if ((count >= 4) and (source[1] == '*'))
        {
            size_t const limit = count - 1;
            for (size_t i = 2; i < limit; ++i)
            {
                if ((source[i] == '*') and (source[i + 1] == '/'))
                {
                    // parsed '/*...*/' characters
                    return i + 1;
                }
            }
        }

        return 0;
    }

    [[nodiscard]] constexpr size_t ConsumeTrivia(std::string_view source) noexcept
    {
        size_t const count = source.size();
        size_t consumed = 0;

        // Consume whitespace or comments until we reach a non-trivia character.
        for (; consumed < count; ++consumed)
        {
            switch (source[consumed])
            {
            case ' ':
            case '\t':
            case '\n':
            case '\r':
                {
                    break;
                }

            case '/':
                {
                    if (size_t const consumedComment = ConsumeLineComment(source.substr(consumed)); consumedComment != 0)
                    {
                        // Consumed line comment.
                        consumed += consumedComment;
                        break;
                    }

                    if (size_t const consumedComment = ConsumeBlockComment(source.substr(consumed)); consumedComment != 0)
                    {
                        // Consumed block comment.
                        consumed += consumedComment;
                        break;
                    }
                }
                break;

            default:
                {
                    return consumed;
                }
            }
        }

        return consumed;
    }

    [[nodiscard]] constexpr size_t ConsumeDigits(std::string_view source) noexcept
    {
        //  digits
        //      digit
        //      digit digits
        //
        //  digit
        //      '0'
        //      onenine
        //
        //  onenine
        //      '1' . '9'

        size_t const count = source.size();

        size_t consumed = 0;

        for (; consumed < count; ++consumed)
        {
            char const first = source[consumed];

            if ((first < '0') or ('9' < first))
            {
                break;
            }
        }

        return consumed;
    }

    [[nodiscard]] constexpr size_t ConsumeInteger(std::string_view source) noexcept
    {
        //  integer
        //      digit
        //      onenine digits
        //      '-' digit
        //      '-' onenine digits

        size_t const count = source.size();
        size_t consumed = 0;

        if ((consumed < count) and (source[consumed] == '-'))
        {
            // Consume sign
            ++consumed;
        }

        if (consumed < count)
        {
            if (source[consumed] == '0')
            {
                return consumed + 1;
            }

            size_t const consumedDigits = ConsumeDigits(source.substr(consumed));

            if (consumedDigits != 0)
            {
                return consumed + consumedDigits;
            }
        }

        return 0;
    }

    [[nodiscard]] constexpr size_t ConsumeFraction(std::string_view source) noexcept
    {
        //  fraction
        //      ""
        //      '.' digits

        size_t const count = source.size();

        if ((count >= 2) and (source[0] == '.'))
        {
            size_t const consumedDigits = ConsumeDigits(source.substr(1));

            if (consumedDigits != 0)
            {
                return 1 + consumedDigits;
            }
        }

        return 0;
    }

    [[nodiscard]] constexpr size_t ConsumeExponent(std::string_view source) noexcept
    {
        //  exponent
        //      ""
        //      'E' sign digits
        //      'e' sign digits

        size_t const count = source.size();

        if (count >= 3)
        {
            char const exponent = source[0];
            char const sign = source[1];

            if (((exponent == 'e') or (exponent == 'E')) and ((sign == '-') or (sign == '+')))
            {
                size_t const consumedDigits = ConsumeDigits(source.substr(2));

                if (consumedDigits != 0)
                {
                    return 2 + consumedDigits;
                }
            }
        }

        return 0;
    }

    [[nodiscard]] constexpr size_t ConsumeNumber(std::string& sink, std::string_view source) noexcept
    {
        //  number
        //      integer fraction exponent

        size_t consumed = ConsumeInteger(source);

        if (consumed != 0)
        {
            consumed += ConsumeFraction(source.substr(consumed));
            consumed += ConsumeExponent(source.substr(consumed));

            sink.assign(source.substr(0, consumed));
        }

        return consumed;
    }

    [[nodiscard]] constexpr int32_t ConsumeHexDigit(char value) noexcept
    {
        if (('0' <= value) and (value <= '9'))
        {
            return value - '0';
        }

        if (('a' <= value) and (value <= 'f'))
        {
            return (value - 'a') + 10;
        }

        if (('A' <= value) and (value <= 'F'))
        {
            return (value - 'A') + 10;
        }

        return -1;
    }

    [[nodiscard]] constexpr bool ConsumeUnicodeChar16(char16_t& result, std::string_view source) noexcept
    {
        size_t const count = source.size();

        if ((count >= 6) and (source[0] == '\\') and (source[1] == 'u'))
        {
            char16_t bits = 0;
            size_t consumed = 2;

            for (; consumed < 6; ++consumed)
            {
                int32_t const n = ConsumeHexDigit(source[consumed]);

                if (n < 0)
                {
                    result = 0;
                    return false;
                }

                bits <<= 4u;
                bits |= static_cast<uint8_t>(n);
            }

            result = bits;
            return true;
        }

        return false;
    }

    [[nodiscard]] constexpr size_t ConsumeUnicodeEscapeSequence(std::string& sink, std::string_view source) noexcept
    {
#if true
        (void)sink;
        (void)source;
#else
        char16_t buffer[2];

        if (ConsumeUnicodeChar16(buffer[0], source))
        {
            switch (UnicodeConvert(sink, std::u16string_view{buffer, 1}, UnicodeConversionType::Strict))
            {
            case UnicodeConversionResult::Success:
                // First '\uAABB' matched properly
                return 6;

            case UnicodeConversionResult::SourceExhausted:
                // Not enough bytes to parse surrogate pair.
                break;

            case UnicodeConversionResult::TargetExhausted:
            case UnicodeConversionResult::SourceIllegal:
                // Failed to match unicode sequence.
                return 0;
            }

            if (ConsumeUnicodeChar16(buffer[1], source.substr(6)))
            {
                switch (UnicodeConvert(sink, std::u16string_view{buffer, 2}, UnicodeConversionType::Strict))
                {
                case UnicodeConversionResult::Success:
                    // Consumed '\uAABB\uCCDD'
                    return 12;

                case UnicodeConversionResult::SourceIllegal:
                case UnicodeConversionResult::SourceExhausted:
                case UnicodeConversionResult::TargetExhausted:
                    // Failed to match unicode sequence.
                    return 0;
                }
            }
        }
#endif
        return 0;
    }

    [[nodiscard]] constexpr size_t ConsumeEscapeSequence(std::string& sink, std::string_view source) noexcept
    {
        if (source.size() >= 2)
        {
            switch (source[1])
            {
            case '"':
                sink.push_back('"');
                return 2;

            case '\\':
                sink.push_back('\\');
                return 2;

            case '/':
                sink.push_back('/');
                return 2;

            case 'b':
                sink.push_back('\b');
                return 2;

            case 'f':
                sink.push_back('\f');
                return 2;

            case 'n':
                sink.push_back('\n');
                return 2;

            case 'r':
                sink.push_back('\r');
                return 2;

            case 't':
                sink.push_back('\t');
                return 2;

            case 'u':
                return ConsumeUnicodeEscapeSequence(sink, source);

            default:
                break;
            }
        }

        // Invalid escape sequence
        return 0;
    }

    [[nodiscard]] inline size_t ConsumeStringCharacter(std::string& sink, std::string_view source) noexcept
    {
        // character
        //     '0020' . '10FFFF' - '"' - '\'
        //     '\' escape

        const char* first = source.data();
        const char* last = first + source.size();

        if (first != last)
        {
            uint8_t const ch = static_cast<uint8_t>(*first);

            if (ch < 0x20)
            {
                return 0;
            }

            if (ch < 0x80u)
            {
                if (ch != '\\')
                {
                    // Handle ascii characters.
                    sink.push_back(static_cast<char>(ch));
                    return 1;
                }
                else
                {
                    return ConsumeEscapeSequence(sink, source);
                }
            }

            const char* it = first;
            if (char32_t codepoint; unicode::Decode(codepoint, it, last) == unicode::ConversionResult::Success)
            {
                // Valid unicode character.
                sink.append(first, static_cast<size_t>(it - first));
                return static_cast<size_t>(it - first);
            }
        }

        return 0;
    }

    [[nodiscard]] static size_t ConsumeString(std::string& sink, std::string_view source) noexcept
    {
        //  string
        //      '"' characters '"'
        //
        //  characters
        //      ""
        //      character characters

        sink.clear();

        size_t const count = source.size();

        if ((count >= 2) and (source[0] == '"'))
        {
            size_t consumed = 1;

            while (consumed < count)
            {
                if (source[consumed] == '"')
                {
                    return consumed + 1;
                }

                size_t const consumedCharacter = ConsumeStringCharacter(sink, source.substr(consumed));

                if (consumedCharacter == 0)
                {
                    return 0;
                }

                consumed += consumedCharacter;
            }
        }

        // If we reached here, string was not terminated properly.
        return 0;
    }
}

namespace weave::json
{
    JsonReader::JsonReader(std::string_view source) noexcept
        : m_Source{source}
    {
        this->m_ReaderState.reserve(32);
    }

    JsonReader::JsonReader(std::string_view source, JsonReaderOptions const& options) noexcept
        : m_Source{source}
        , m_Options{options}
    {
        this->m_ReaderState.reserve(32);
    }


    JsonReader::Token JsonReader::ReadNext() noexcept
    {
        if (std::string_view const tail = this->GetSourceTail(); not tail.empty())
        {
            if (size_t const consumed = (this->m_Options.AllowComments ? impl::ConsumeTrivia : impl::ConsumeWhitespace)(tail); consumed != 0)
            {
                this->Consume(consumed);
            }
        }
        else
        {
            return Token::EndOfStream;
        }

        if (std::string_view const tail = this->GetSourceTail(); not tail.empty())
        {
            switch (tail.front())
            {
            case ',':
                return Token::ValueSeparator;

            case ':':
                return Token::KeySeparator;

            case '{':
                return Token::StartObject;

            case '}':
                return Token::EndObject;

            case '[':
                return Token::StartArray;

            case ']':
                return Token::EndArray;

            case '-':
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                if (size_t const consumed = impl::ConsumeNumber(this->m_CurrentValue, tail); consumed != 0)
                {
                    this->Consume(consumed);
                    return Token::NumberValue;
                }
                break;

            case '"':
                if (size_t const consumed = impl::ConsumeString(this->m_CurrentValue, tail); consumed != 0)
                {
                    this->Consume(consumed);
                    return Token::StringValue;
                }
                break;

            case 't':
                if (tail.starts_with("true"))
                {
                    this->Consume(4);
                    return Token::TrueValue;
                }
                break;

            case 'f':
                if (tail.starts_with("false"))
                {
                    this->Consume(5);
                    return Token::FalseValue;
                }
                break;

            case 'n':
                if (tail.starts_with("null"))
                {
                    this->Consume(4);
                    return Token::NullValue;
                }
                break;

            default:
                break;
            }
        }
        else
        {
            return Token::EndOfStream;
        }

        return Token::Error;
    }

    bool JsonReader::ReadAfterValue(JsonEvent event) noexcept
    {
        if (not this->m_ReaderState.empty())
        {
            ReaderState const state = this->m_ReaderState.back();
            this->m_ReaderState.pop_back();

            switch (state)
            {
            case ReaderState::FirstObjectKey:
            case ReaderState::NextObjectKey:
                {
                    Token const next = this->ReadNext();
                    if (next == Token::KeySeparator)
                    {
                        this->Consume(1);
                        this->m_ReaderState.push_back(ReaderState::Value);

                        if (event == JsonEvent::StringValue)
                        {
                            this->m_CurrentEvent = JsonEvent::Key;
                            return true;
                        }

                        // Object key must be string
                        this->m_CurrentEvent = JsonEvent::Error;
                        return false;
                    }

                    // Key should be followed by ':' separator
                    this->m_CurrentEvent = JsonEvent::Error;
                    return false;
                }

            case ReaderState::Value:
                {
                    Token const next = this->ReadNext();
                    if (next == Token::ValueSeparator)
                    {
                        this->Consume(1);
                        this->m_ReaderState.push_back(ReaderState::NextObjectKey);
                        this->m_CurrentEvent = event;
                        return true;
                    }

                    if (next == Token::EndObject)
                    {
                        this->m_ReaderState.push_back(ReaderState::LastObjectKey);
                        this->m_CurrentEvent = event;
                        return true;
                    }

                    // Object must be followed by a comma at object end.
                    this->m_CurrentEvent = JsonEvent::Error;
                    return false;
                }

            case ReaderState::FirstArray:
            case ReaderState::NextArray:
                {
                    Token const next = this->ReadNext();
                    if (next == Token::ValueSeparator)
                    {
                        this->Consume(1);
                        this->m_ReaderState.push_back(ReaderState::NextArray);
                        this->m_CurrentEvent = event;
                        return true;
                    }

                    if (next == Token::EndArray)
                    {
                        this->m_ReaderState.push_back(ReaderState::LastArray);
                        this->m_CurrentEvent = event;
                        return true;
                    }

                    // Array values should be followe by a comma or the array end.
                    this->m_CurrentEvent = JsonEvent::Error;
                    return false;
                }

            case ReaderState::LastObjectKey:
                // Json object elements should be separated by commas
                this->m_CurrentEvent = JsonEvent::Error;
                return false;

            case ReaderState::LastArray:
                // Json array elements should be separated by commas
                this->m_CurrentEvent = JsonEvent::Error;
                return false;
            }
        }
        else
        {
            if (this->m_ElementRead)
            {
                this->m_CurrentEvent = JsonEvent::Error;
                return false;
            }

            this->m_ElementRead = true;
            this->m_CurrentEvent = event;
            return true;
        }

        // Trailing content
        this->m_CurrentEvent = JsonEvent::Error;
        return false;
    }

    bool JsonReader::Next() noexcept
    {
        this->m_CurrentValue.clear();

        switch (this->ReadNext())
        {
        case Token::None:
        case Token::Error:
        case Token::KeySeparator:
        case Token::ValueSeparator:
            {
                this->m_CurrentEvent = JsonEvent::Error;
                return false;
            }

        case Token::EndOfStream:
            {
                this->m_CurrentEvent = JsonEvent::EndOfStream;
                return false;
            }

        case Token::StartObject:
            {
                this->Consume(1);
                this->m_ReaderState.push_back(ReaderState::FirstObjectKey);
                this->m_CurrentEvent = JsonEvent::StartObject;
                return true;
            }
        case Token::EndObject:
            {
                this->Consume(1);
                if (not this->m_ReaderState.empty())
                {
                    ReaderState const state = this->m_ReaderState.back();
                    m_ReaderState.pop_back();

                    if ((state == ReaderState::FirstObjectKey) or (state == ReaderState::LastObjectKey) or ((state == ReaderState::NextObjectKey) and this->m_Options.AllowTrailingCommas))
                    {
                        return ReadAfterValue(JsonEvent::EndObject);
                    }
                }

                // Closing not opened object.
                this->m_CurrentEvent = JsonEvent::Error;
                return false;
            }
        case Token::StartArray:
            {
                this->Consume(1);
                this->m_ReaderState.push_back(ReaderState::FirstArray);
                this->m_CurrentEvent = JsonEvent::StartArray;
                return true;
            }
        case Token::EndArray:
            {
                this->Consume(1);
                if (not this->m_ReaderState.empty())
                {
                    ReaderState const state = this->m_ReaderState.back();
                    m_ReaderState.pop_back();

                    if ((state == ReaderState::FirstArray) or (state == ReaderState::LastArray) or ((state == ReaderState::NextArray) and this->m_Options.AllowTrailingCommas))
                    {
                        return ReadAfterValue(JsonEvent::EndArray);
                    }
                }

                this->m_CurrentEvent = JsonEvent::Error;
                return false;
            }
        case Token::StringValue:
            {
                return ReadAfterValue(JsonEvent::StringValue);
            }
        case Token::NumberValue:
            {
                return ReadAfterValue(JsonEvent::NumberValue);
            }
        case Token::TrueValue:
            {
                return ReadAfterValue(JsonEvent::TrueValue);
            }
        case Token::FalseValue:
            {
                return ReadAfterValue(JsonEvent::FalseValue);
            }
        case Token::NullValue:
            {
                return ReadAfterValue(JsonEvent::NullValue);
            }
        }

        this->m_CurrentEvent = JsonEvent::Error;
        return false;
    }
}
