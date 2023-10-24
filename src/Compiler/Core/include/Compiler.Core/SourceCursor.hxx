#pragma once
#include "Compiler.Core/Source.hxx"

#include <string_view>
#include <span>
#include <limits>

namespace Weave
{
    class SourceCursor final
    {
    private:
        const char* m_Current;
        const char* m_Next;
        const char* m_Start;
        const char* m_First;
        const char* m_Last;
        char32_t m_Value;

    private:
        static constexpr char32_t Invalid = std::numeric_limits<char32_t>::max();

    public:
        explicit SourceCursor(std::string_view v)
            : m_Current{v.data()}
            , m_Next{v.data()}
            , m_Start{v.data()}
            , m_First{v.data()}
            , m_Last{v.data() + v.size()}
            , m_Value{Invalid}
        {
            this->Advance();
        }

        void Start()
        {
            // Remember where current range starts.
            this->m_Start = this->m_Current;
        }

        void Reset(SourcePosition const& position)
        {
            this->m_Next = this->m_First + position.Offset;
            this->Advance();
        }

        [[nodiscard]] SourcePosition GetCurrentPosition() const
        {
            return SourcePosition{
                static_cast<size_t>(this->m_Current - this->m_First),
            };
        }

        [[nodiscard]] SourcePosition GetNextPosition() const
        {
            return SourcePosition{
                static_cast<size_t>(this->m_Next - this->m_First),
            };
        }

        [[nodiscard]] SourceSpan GetSpan() const
        {
            return SourceSpan{
                .Start = SourcePosition{static_cast<size_t>(this->m_Start - this->m_First)},
                .End = SourcePosition{static_cast<size_t>(this->m_Current - this->m_First)},
            };
        }

        [[nodiscard]] SourceSpan GetSpanForCurrent() const
        {
            return SourceSpan{
                .Start = SourcePosition{static_cast<size_t>(this->m_Current - this->m_First)},
                .End = SourcePosition{static_cast<size_t>(this->m_Next - this->m_First)},
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
            return this->m_Current == this->m_Last;
        }

        [[nodiscard]] bool IsValid() const
        {
            return this->m_Value != Invalid;
        }

        [[nodiscard]] char32_t Peek() const
        {
            return this->m_Value;
        }

        [[nodiscard]] std::string_view PeekAsStringView() const
        {
            return std::string_view{this->m_Current, this->m_Next};
        }

        [[nodiscard]] SourceCursor Next() const
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
            if (this->m_Value == c)
            {
                this->Advance();
                return true;
            }

            return false;
        }

        template <typename PredicateT>
        [[nodiscard]] bool FirstIf(PredicateT&& predicate)
        {
            if (predicate(this->m_Value))
            {
                this->Advance();
                return true;
            }

            return false;
        }

        [[nodiscard]] bool Skip(char32_t c)
        {
            if (this->m_Value == c)
            {
                do
                {
                    this->Advance();
                } while (this->m_Value == c);

                return true;
            }

            return false;
        }

        template <typename PredicateT>
        [[nodiscard]] bool SkipIf(PredicateT&& predicate)
        {
            if (predicate(this->m_Value))
            {
                do
                {
                    this->Advance();
                } while (predicate(this->m_Value));

                return true;
            }

            return false;
        }

        template <typename PredicateT>
        [[nodiscard]] bool SkipMaxIf(size_t count, PredicateT&& predicate)
        {
            size_t consumed = 0;

            while ((consumed < count) and (predicate(this->m_Value)))
            {
                this->Advance();
                ++consumed;
            }

            return consumed;
        }

        [[nodiscard]] size_t Count(char32_t c)
        {
            size_t count = 0;

            while (this->m_Value == c)
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

            while (predicate(this->m_Value))
            {
                this->Advance();
                ++count;
            }

            return count;
        }

        void Discard(char32_t c)
        {
            while (this->m_Value == c)
            {
                this->Advance();
            }
        }

        template <typename PredicateT>
        void DiscardIf(PredicateT&& predicate)
        {
            while (predicate(this->m_Value))
            {
                this->Advance();
            }
        }
    };
}
