#pragma once
#include "Weave.Core/Source.hxx"
#include "Weave.Core/Function.hxx"

#include <fmt/format.h>

#include <limits>
#include <compare>
#include <vector>
#include <cassert>

namespace Weave
{
    enum class DiagnosticLevel
    {
        Error,
        Warning,
        Info,
        Hint,
    };
}

template <>
struct fmt::formatter<Weave::DiagnosticLevel> : formatter<std::string_view>
{
    constexpr auto format(Weave::DiagnosticLevel const& value, auto& context)
    {
        std::string_view result = "unknown";

        switch (value)
        {
        case Weave::DiagnosticLevel::Error:
            result = "error";
            break;

        case Weave::DiagnosticLevel::Warning:
            result = "warning";
            break;

        case Weave::DiagnosticLevel::Info:
            result = "info";
            break;

        case Weave::DiagnosticLevel::Hint:
            result = "hint";
            break;
        }

        return formatter<std::string_view>::format(result, context);
    }
};

namespace Weave
{
    class DiagnosticSink final
    {
    public:
        struct Handle final
        {
            size_t Value{std::numeric_limits<size_t>::max()};

            [[nodiscard]] constexpr auto operator<=>(Handle const&) const = default;
        };

        struct Entry final
        {
            SourceSpan Source{};
            DiagnosticLevel Level{};
            std::string Message{};
        };

        struct Links final
        {
            Handle Parent{}; // parent of this node
            Handle First{}; // first child of this node
            Handle Last{};
            Handle Next{}; // next sibling of this node
            Handle Prev{};
        };

    private:
        std::vector<Entry> m_Entries{};
        std::vector<Links> m_Links{};

    private:
        void LinkBack(Handle parent, Handle child)
        {
            auto& parentLinks = this->m_Links[parent.Value];
            auto& childLinks = this->m_Links[child.Value];

            if (parentLinks.Last == parent)
            {
                assert(parentLinks.First == parent);

                parentLinks.First = child;
                parentLinks.Last = child;

                childLinks.Next = parent;
                childLinks.Prev = parent;
            }
            else
            {
                // Parent has at least one child. Insert after last child.

                assert(this->m_Links[parentLinks.First.Value].Prev == parent);
                assert(this->m_Links[parentLinks.Last.Value].Next == parent);

                this->m_Links[parentLinks.Last.Value].Next = child;
                childLinks.Prev = parentLinks.Last;
                childLinks.Next = parent;
                parentLinks.Last = child;
            }

            assert(childLinks.Parent == parent);
        }

    public:
        Handle Add(SourceSpan const& source, DiagnosticLevel level, std::string&& message)
        {
            Handle result{this->m_Entries.size()};
            this->m_Entries.emplace_back(source, level, std::move(message));
            this->m_Links.emplace_back(result, result, result, result, result);
            return result;
        }

        Handle Add(Handle parent, SourceSpan const& source, DiagnosticLevel level, std::string&& message)
        {
            Handle result{this->m_Entries.size()};
            this->m_Entries.emplace_back(source, level, std::move(message));
            this->m_Links.emplace_back(parent, result, result, parent, parent);
            this->LinkBack(parent, result);
            return result;
        }

        Handle AddError(SourceSpan const& source, std::string&& message)
        {
            return this->Add(source, DiagnosticLevel::Error, std::move(message));
        }

        Handle AddWarning(SourceSpan const& source, std::string&& message)
        {
            return this->Add(source, DiagnosticLevel::Warning, std::move(message));
        }

        Handle AddInfo(SourceSpan const& source, std::string&& message)
        {
            return this->Add(source, DiagnosticLevel::Info, std::move(message));
        }

        Handle AddHint(SourceSpan const& source, std::string&& message)
        {
            return this->Add(source, DiagnosticLevel::Hint, std::move(message));
        }

        Handle AddError(Handle parent, SourceSpan const& source, std::string&& message)
        {
            return this->Add(parent, source, DiagnosticLevel::Error, std::move(message));
        }

        Handle AddWarning(Handle parent, SourceSpan const& source, std::string&& message)
        {
            return this->Add(parent, source, DiagnosticLevel::Warning, std::move(message));
        }

        Handle AddInfo(Handle parent, SourceSpan const& source, std::string&& message)
        {
            return this->Add(parent, source, DiagnosticLevel::Info, std::move(message));
        }

        Handle AddHint(Handle parent, SourceSpan const& source, std::string&& message)
        {
            return this->Add(parent, source, DiagnosticLevel::Hint, std::move(message));
        }

        Entry const* TryGet(Handle handle) const
        {
            if (handle.Value < this->m_Entries.size())
            {
                return &this->m_Entries[handle.Value];
            }

            return nullptr;
        }

        void EnumerateChildren(Handle handle, FunctionRef<void(Handle children)> callback) const
        {
            for (Handle it = this->m_Links[handle.Value].First; it != handle; it = this->m_Links[it.Value].Next)
            {
                callback(it);
            }
        }

        void Enumerate(FunctionRef<void(Entry const& entry)> callback) const
        {
            for (auto const& m_Entrie : this->m_Entries)
            {
                callback(m_Entrie);
            }
        }

        void EnumerateRoots(FunctionRef<void(Entry const& entry, Handle handle)> callback) const
        {
            for (size_t i = 0; i < this->m_Entries.size(); ++i)
            {
                Links const& links = this->m_Links[i];

                if (links.Parent.Value == i)
                {
                    // This is root
                    Entry const& entry = this->m_Entries[i];
                    callback(entry, Handle{i});
                }
            }
        }
    };
}
