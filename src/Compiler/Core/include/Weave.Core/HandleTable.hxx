#pragma once
#include <compare>
#include <limits>
#include <vector>
#include <cstddef>

namespace Weave
{
    template <typename T>
    class HandleTable
    {
    public:
        struct Handle final
        {
            size_t Value{std::numeric_limits<size_t>::max()};

            [[nodiscard]] constexpr auto operator<=>(Handle const&) const = default;
        };

        class Iterator
        {
        private:
            Handle _current{};

        public:
            Iterator() = default;

            explicit Iterator(Handle handle)
                : _current{handle}
            {
            }

        public:
            Iterator& operator++()
            {
                ++this->_current.Value;
                return *this;
            }


            [[nodiscard]] Iterator operator++(int)
            {
                Iterator copy{*this};
                ++(*this);
                return copy;
            }

            Iterator& operator--()
            {
                --this->_current.Value;
                return *this;
            }

            [[nodiscard]] Iterator operator--(int)
            {
                Iterator copy{*this};
                --(*this);
                return copy;
            }

            [[nodiscard]] Handle operator*() const
            {
                return this->_current;
            }

            [[nodiscard]] constexpr auto operator<=>(Iterator const&) const = default;
        };

        Iterator begin() const
        {
            return Iterator{Handle{0}};
        }

        Iterator end() const
        {
            return Iterator{Handle{this->_items.size()}};
        }

        template <typename... Us>
        Handle add(Us&&... us)
        {
            Handle const result{this->_items.size()};
            this->_items.emplace_back(std::forward<Us>(us)...);
            return result;
        }

        T const* get(Handle handle) const
        {
            if (handle.Value < this->_items.size())
            {
                return &this->_items[handle.Value];
            }

            return nullptr;
        }

    private:
        std::vector<T> _items{};
    };
}
