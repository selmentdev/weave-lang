#pragma once
#include "weave/source/Source.hxx"

#include <string_view>
#include <string>
#include <vector>

namespace weave::json
{
    enum class JsonEvent : uint8_t
    {
        None,
        Error,
        EndOfStream,
        Key,
        TrueValue,
        FalseValue,
        NullValue,
        StringValue,
        NumberValue,
        StartObject,
        EndObject,
        StartArray,
        EndArray,
    };

    struct JsonReaderOptions final
    {
        bool AllowTrailingCommas = false;
        bool AllowComments = false;
    };

    class JsonReader final
    {
    private:
        enum class ReaderState : uint8_t
        {
            FirstArray,
            NextArray,
            LastArray,
            FirstObjectKey,
            NextObjectKey,
            LastObjectKey,
            Value,
        };

        enum class Token : uint8_t
        {
            None,
            Error,
            EndOfStream,
            StartObject,
            EndObject,
            StartArray,
            EndArray,
            KeySeparator,
            ValueSeparator,
            StringValue,
            NumberValue,
            TrueValue,
            FalseValue,
            NullValue,
        };

    private:
        std::string_view m_Source;
        std::vector<ReaderState> m_ReaderState{};
        std::string m_CurrentValue{};
        source::SourceSpan m_CurrentToken{};
        JsonEvent m_CurrentEvent{};
        bool m_ElementRead{false};

        JsonReaderOptions m_Options{};

    private:
        Token ReadNext() noexcept;

        bool ReadAfterValue(JsonEvent event) noexcept;

        constexpr std::string_view GetSourceTail() const noexcept
        {
            return this->m_Source.substr(this->m_CurrentToken.End.Offset);
        }

        constexpr void Consume(size_t length) noexcept
        {
            this->m_CurrentToken.Start = this->m_CurrentToken.End;
            this->m_CurrentToken.End.Offset += static_cast<uint32_t>(length);
        }

    public:
        explicit JsonReader(std::string_view source) noexcept;
        explicit JsonReader(std::string_view source, JsonReaderOptions const& options) noexcept;

        ~JsonReader() noexcept = default;

        JsonReader(JsonReader const&) = delete;
        JsonReader(JsonReader&&) = delete;
        JsonReader& operator=(JsonReader const&) = delete;
        JsonReader& operator=(JsonReader&&) = delete;

    public:
        bool Next() noexcept;

        constexpr std::string_view GetValue() const noexcept
        {
            return this->m_CurrentValue;
        }

        constexpr JsonEvent GetEvent() const noexcept
        {
            return this->m_CurrentEvent;
        }
    };
}
