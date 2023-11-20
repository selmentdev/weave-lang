#pragma once
#include "weave/source/Source.hxx"

#include <string_view>
#include <span>
#include <limits>

namespace weave::source
{
    class SourceCursor final
    {
    private:
        const char* _current;
        const char* _next;
        const char* _start;
        const char* _first;
        const char* _last;
        char32_t _value;

    private:
        static constexpr char32_t Invalid = std::numeric_limits<char32_t>::max();

    public:
        explicit SourceCursor(std::string_view v)
            : _current{v.data()}
            , _next{v.data()}
            , _start{v.data()}
            , _first{v.data()}
            , _last{v.data() + v.size()}
            , _value{Invalid}
        {
            this->Advance();
        }

        void Start()
        {
            // Remember where current range starts.
            this->_start = this->_current;
        }

        void Reset(SourcePosition const& position)
        {
            this->_next = this->_first + position.Offset;
            this->Advance();
        }

        [[nodiscard]] SourcePosition GetCurrentPosition() const
        {
            return SourcePosition{
                static_cast<uint32_t>(this->_current - this->_first),
            };
        }

        [[nodiscard]] SourcePosition GetNextPosition() const
        {
            return SourcePosition{
                static_cast<uint32_t>(this->_next - this->_first),
            };
        }

        [[nodiscard]] SourceSpan GetSpan() const
        {
            return SourceSpan{
                .Start = SourcePosition{static_cast<uint32_t>(this->_start - this->_first)},
                .End = SourcePosition{static_cast<uint32_t>(this->_current - this->_first)},
            };
        }

        [[nodiscard]] SourceSpan GetSpanForCurrent() const
        {
            return SourceSpan{
                .Start = SourcePosition{static_cast<uint32_t>(this->_current - this->_first)},
                .End = SourcePosition{static_cast<uint32_t>(this->_next - this->_first)},
            };
        }

        [[nodiscard]] SourceSpan GetSpanToCurrent(SourcePosition const& start) const
        {
            return SourceSpan{
                .Start = start,
                .End = this->GetCurrentPosition(),
            };
        }

        [[nodiscard]] SourceSpan GetSpanToNext(SourcePosition const& start) const
        {
            return SourceSpan{
                .Start = start,
                .End = this->GetNextPosition(),
            };
        }

        [[nodiscard]] bool IsEnd() const
        {
            return this->_current == this->_last;
        }

        [[nodiscard]] bool IsValid() const
        {
            return this->_value != Invalid;
        }

        [[nodiscard]] char32_t Peek() const
        {
            return this->_value;
        }

        [[nodiscard]] std::string_view PeekAsStringView() const
        {
            return std::string_view{this->_current, this->_next};
        }

        [[nodiscard]] SourceCursor NextCursor() const
        {
            SourceCursor cursor{*this};
            cursor.Advance();
            return cursor;
        }

        void Advance();

        [[nodiscard]] bool StartsWith(std::u32string_view n)
        {
            if (not n.empty())
            {
                SourcePosition const start = this->GetCurrentPosition();

                for (char32_t const ch : n)
                {
                    if (not this->First(ch))
                    {
                        this->Reset(start);
                        return false;
                    }
                }
            }

            return true;
        }

        [[nodiscard]] bool First(char32_t c)
        {
            if (this->_value == c)
            {
                this->Advance();
                return true;
            }

            return false;
        }

        template <typename PredicateT>
        [[nodiscard]] bool FirstIf(PredicateT&& predicate)
        {
            if (predicate(this->_value))
            {
                this->Advance();
                return true;
            }

            return false;
        }

        [[nodiscard]] bool Skip(char32_t c)
        {
            if (this->_value == c)
            {
                do
                {
                    this->Advance();
                } while (this->_value == c);

                return true;
            }

            return false;
        }

        template <typename PredicateT>
        [[nodiscard]] bool SkipIf(PredicateT&& predicate)
        {
            if (predicate(this->_value))
            {
                do
                {
                    this->Advance();
                } while (predicate(this->_value));

                return true;
            }

            return false;
        }

        template <typename PredicateT>
        [[nodiscard]] bool SkipMaxIf(size_t count, PredicateT&& predicate)
        {
            size_t consumed = 0;

            while ((consumed < count) and (predicate(this->_value)))
            {
                this->Advance();
                ++consumed;
            }

            return consumed;
        }

        [[nodiscard]] size_t Count(char32_t c)
        {
            size_t count = 0;

            while (this->_value == c)
            {
                this->Advance();
                ++count;
            }

            return count;
        }

        template <typename PredicateT>
        [[nodiscard]] size_t CountIf(PredicateT&& predicate)
        {
            size_t count = 0;

            while (predicate(this->_value))
            {
                this->Advance();
                ++count;
            }

            return count;
        }

        void Discard(char32_t c)
        {
            while (this->_value == c)
            {
                this->Advance();
            }
        }

        template <typename PredicateT>
        void DiscardIf(PredicateT&& predicate)
        {
            while (predicate(this->_value))
            {
                this->Advance();
            }
        }
    };
}
