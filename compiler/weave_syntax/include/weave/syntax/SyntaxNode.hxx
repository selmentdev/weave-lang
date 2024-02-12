#pragma once
#include "weave/bugcheck/Assert.hxx"
#include "weave/syntax/SyntaxKind.hxx"
#include "weave/source/Source.hxx"

#include <span>

namespace weave::syntax
{
    struct SyntaxNode
    {
        SyntaxKind Kind;

        explicit constexpr SyntaxNode(SyntaxKind kind)
            : Kind{kind}
        {
        }

        [[nodiscard]] constexpr bool Is(SyntaxKind kind) const
        {
            return this->Kind == kind;
        }

        template <typename T>
        [[nodiscard]] constexpr T const* TryCast() const
        {
            return T::ClassOf(this) ? static_cast<T const*>(this) : nullptr;
        }

        template <typename T>
        [[nodiscard]] constexpr T* TryCast()
        {
            return T::ClassOf(this) ? static_cast<T*>(this) : nullptr;
        }
    };

    
    class SyntaxList : public SyntaxNode
    {
        // Optimization idea: we can store elements in memory after the SyntaxList object
    public:
        static constexpr bool ClassOf(SyntaxKind kind)
        {
            return kind == SyntaxKind::SyntaxList;
        }

        static constexpr bool ClassOf(SyntaxNode const* node)
        {
            return node->Is(SyntaxKind::SyntaxList);
        }

    private:
        std::span<SyntaxNode*> _elements{};

    public:
        explicit constexpr SyntaxList(std::span<SyntaxNode*> elements)
            : SyntaxNode{SyntaxKind::SyntaxList}
            , _elements{elements}
        {
        }

    public:
        [[nodiscard]] constexpr SyntaxNode** GetElements() const
        {
            return this->_elements.data();
        }

        [[nodiscard]] constexpr size_t GetCount() const
        {
            return this->_elements.size();
        }

        [[nodiscard]] constexpr SyntaxNode* Get(size_t index) const
        {
            return this->_elements[index];
        }
    };
}


namespace weave::syntax
{
    template <typename T>
    struct SyntaxListView
    {
    private:
        SyntaxList* _node;

    public:
        SyntaxListView() = default;

        explicit constexpr SyntaxListView(SyntaxList* node)
            : _node{node}
        {
        }

        SyntaxListView(SyntaxListView const&) = default;
        SyntaxListView(SyntaxListView&&) = default;
        SyntaxListView& operator=(SyntaxListView const&) = default;
        SyntaxListView& operator=(SyntaxListView&&) = default;

    public:
        [[nodiscard]] constexpr SyntaxList* GetNode() const
        {
            return this->_node;
        }

        [[nodiscard]] constexpr size_t GetCount() const
        {
            if (this->_node == nullptr)
            {
                return 0;
            }

            return this->_node->GetCount();
        }

        [[nodiscard]] constexpr T* GetElement(size_t index) const
        {
            if (this->_node == nullptr)
            {
                return nullptr;
            }

            return static_cast<T*>(this->_node->GetElements()[index]);
        }

        template <typename OtherT>
        [[nodiscard]] constexpr SyntaxListView<OtherT> Cast() const
        {
            return SyntaxListView<OtherT>{this->_node};
        }
    };
}

// Forward declare all syntax nodes

namespace weave::syntax
{
    class NameSyntax;
    class ExpressionSyntax;
    class DeclarationSyntax;
    class MemberDeclarationSyntax;
    class StatementSyntax;
    class TypeSyntax;
    class MatchClauseSyntax;
    class PatternSyntax;
    class GenericParameterSyntax;

#define WEAVE_SYNTAX_NODE(name, spelling) class name;
#include "weave/syntax/SyntaxKind.inl"
}
