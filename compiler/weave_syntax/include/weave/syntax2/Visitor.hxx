#pragma once
#include "weave/syntax2/SyntaxNode.hxx"
#include "weave/syntax2/SyntaxToken.hxx"
#include "weave/bugcheck/BugCheck.hxx"

namespace weave::syntax2
{
    template <typename VisitorT, typename ResultT, typename... ArgsT>
    class SyntaxVisitor
    {
    public:
        ResultT VisitToken(SyntaxToken* token, ArgsT&&... args)
        {
            return VisitDefault(token, std::forward<ArgsT>(args)...);
        }

        ResultT VisitTrivia(SyntaxTrivia* trivia, ArgsT&&... args)
        {
            return VisitDefault(trivia, std::forward<ArgsT>(args)...);
        }

        ResultT VisitDefault(SyntaxNode* node, ArgsT&&... args)
        {
            return ResultT{};
        }

#define WEAVE_SYNTAX_NODE(name, value, spelling) \
    ResultT Visit##Name(name* node, ArgsT&&... args) \
    { \
        return VisitDefault(node, std::forward<ArgsT>(args)...); \
    }
#include "weave/syntax2/SyntaxKind.inl"
    };

    template <typename VisitorT, typename ResultT, typename... ArgsT>
    ResultT Visit(SyntaxNode& node, VisitorT& visitor, ArgsT&&... args)
    {
        SyntaxKind const kind = node.Kind;

        if (SyntaxKindTraits::IsSyntaxToken(kind))
        {
            return visitor.VisitToken(static_cast<SyntaxToken&>(node), std::forward<ArgsT>(args)...);
        }

        switch (kind)
        {
#define WEAVE_SYNTAX_NODE(name, value, spelling) \
    case SyntaxKind::name: \
        return visitor.Visit##Name(static_cast<name&>(node), std::forward<ArgsT>(args)...);
#include "weave/syntax2/SyntaxKind.inl"

        default:
            break;
        }

        WEAVE_BUGCHECK("Invalid node kind");
    }
}
