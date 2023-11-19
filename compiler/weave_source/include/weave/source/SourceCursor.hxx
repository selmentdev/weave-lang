#pragma once
#include "weave/source/source.hxx"

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
            this->advance();
        }

        void start()
        {
            // Remember where current range starts.
            this->_start = this->_current;
        }

        void reset(SourcePosition const& position)
        {
            this->_next = this->_first + position.offset;
            this->advance();
        }

        [[nodiscard]] SourcePosition get_current_position() const
        {
            return SourcePosition{
                static_cast<uint32_t>(this->_current - this->_first),
            };
        }

        [[nodiscard]] SourcePosition get_next_position() const
        {
            return SourcePosition{
                static_cast<uint32_t>(this->_next - this->_first),
            };
        }

        [[nodiscard]] SourceSpan get_span() const
        {
            return SourceSpan{
                .start = SourcePosition{static_cast<uint32_t>(this->_start - this->_first)},
                .end = SourcePosition{static_cast<uint32_t>(this->_current - this->_first)},
            };
        }

        [[nodiscard]] SourceSpan get_span_for_current() const
        {
            return SourceSpan{
                .start = SourcePosition{static_cast<uint32_t>(this->_current - this->_first)},
                .end = SourcePosition{static_cast<uint32_t>(this->_next - this->_first)},
            };
        }

        [[nodiscard]] SourceSpan get_span_to_current(SourcePosition const& start) const
        {
            return SourceSpan{
                .start = start,
                .end = this->get_current_position(),
            };
        }

        [[nodiscard]] SourceSpan get_span_to_next(SourcePosition const& start) const
        {
            return SourceSpan{
                .start = start,
                .end = this->get_next_position(),
            };
        }

        [[nodiscard]] bool is_end() const
        {
            return this->_current == this->_last;
        }

        [[nodiscard]] bool is_valid() const
        {
            return this->_value != Invalid;
        }

        [[nodiscard]] char32_t peek() const
        {
            return this->_value;
        }

        [[nodiscard]] std::string_view peek_as_string_view() const
        {
            return std::string_view{this->_current, this->_next};
        }

        [[nodiscard]] SourceCursor next() const
        {
            SourceCursor cursor{*this};
            cursor.advance();
            return cursor;
        }

        void advance();

        [[nodiscard]] bool starts_with(std::u32string_view n)
        {
            if (not n.empty())
            {
                SourcePosition const start = this->get_current_position();

                for (char32_t const ch : n)
                {
                    if (not this->first(ch))
                    {
                        this->reset(start);
                        return false;
                    }
                }
            }

            return true;
        }

        [[nodiscard]] bool first(char32_t c)
        {
            if (this->_value == c)
            {
                this->advance();
                return true;
            }

            return false;
        }

        template <typename PredicateT>
        [[nodiscard]] bool first_if(PredicateT&& predicate)
        {
            if (predicate(this->_value))
            {
                this->advance();
                return true;
            }

            return false;
        }

        [[nodiscard]] bool skip(char32_t c)
        {
            if (this->_value == c)
            {
                do
                {
                    this->advance();
                } while (this->_value == c);

                return true;
            }

            return false;
        }

        template <typename PredicateT>
        [[nodiscard]] bool skip_if(PredicateT&& predicate)
        {
            if (predicate(this->_value))
            {
                do
                {
                    this->advance();
                } while (predicate(this->_value));

                return true;
            }

            return false;
        }

        template <typename PredicateT>
        [[nodiscard]] bool skip_max_if(size_t count, PredicateT&& predicate)
        {
            size_t consumed = 0;

            while ((consumed < count) and (predicate(this->_value)))
            {
                this->advance();
                ++consumed;
            }

            return consumed;
        }

        [[nodiscard]] size_t count(char32_t c)
        {
            size_t count = 0;

            while (this->_value == c)
            {
                this->advance();
                ++count;
            }

            return count;
        }

        template <typename PredicateT>
        [[nodiscard]] size_t count_if(PredicateT&& predicate)
        {
            size_t count = 0;

            while (predicate(this->_value))
            {
                this->advance();
                ++count;
            }

            return count;
        }

        void discard(char32_t c)
        {
            while (this->_value == c)
            {
                this->advance();
            }
        }

        template <typename PredicateT>
        void discard_if(PredicateT&& predicate)
        {
            while (predicate(this->_value))
            {
                this->advance();
            }
        }
    };
}
