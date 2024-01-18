// ReSharper disable CppClangTidyClangDiagnosticExtraSemi
#pragma once
#include "weave/syntax/SyntaxNode.hxx"
#include "weave/syntax/SyntaxToken.hxx"

#define WEAVE_DEFINE_SYNTAX_NODE(k) \
public: \
    static constexpr bool ClassOf(SyntaxKind kind) \
    { \
        return kind == SyntaxKind::k; \
    } \
    static constexpr bool ClassOf(SyntaxNode const* node) \
    { \
        return ClassOf(node->Kind); \
    }

namespace weave::syntax
{

    class DeclarationSyntax : public SyntaxNode
    {
    public:
        explicit constexpr DeclarationSyntax(SyntaxKind kind)
            : SyntaxNode{kind}
        {
        }
    };

    class MemberDeclarationSyntax : public DeclarationSyntax
    {
    public:
        explicit constexpr MemberDeclarationSyntax(SyntaxKind kind)
            : DeclarationSyntax{kind}
        {
        }
    };

    class IncompleteDeclarationSyntax : public MemberDeclarationSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(IncompleteDeclarationSyntax);

    public:
        SyntaxListView<AttributeListSyntax> Attributes{};
        SyntaxListView<SyntaxToken> Modifiers{};
        SyntaxNode* Type{};

    public:
        explicit constexpr IncompleteDeclarationSyntax()
            : MemberDeclarationSyntax{SyntaxKind::IncompleteDeclarationSyntax}
        {
        }
    };

    class UsingDirectiveSyntax : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(UsingDirectiveSyntax);

    public:
        SyntaxToken* UsingKeyword{};
        NameSyntax* Name{};
        SyntaxToken* SemicolonToken{};

    public:
        explicit constexpr UsingDirectiveSyntax()
            : SyntaxNode{SyntaxKind::UsingDirectiveSyntax}
        {
        }
    };

    class CompilationUnitSyntax : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(CompilationUnitSyntax);

    public:
        SyntaxListView<AttributeListSyntax> AttributeLists{};
        SyntaxListView<UsingDirectiveSyntax> Usings{};
        SyntaxListView<MemberDeclarationSyntax> Members{};
        SyntaxToken* EndOfFileToken{};

    public:
        explicit constexpr CompilationUnitSyntax()
            : SyntaxNode{SyntaxKind::CompilationUnitSyntax}
        {
        }
    };

    class NamespaceDeclarationSyntax : public MemberDeclarationSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(NamespaceDeclarationSyntax);

    public:
        SyntaxListView<AttributeListSyntax> Attributes{};
        SyntaxListView<SyntaxToken> Modifiers{};
        SyntaxToken* NamespaceKeyword{};
        NameSyntax* Name{};
        SyntaxToken* OpenBraceToken{};
        SyntaxListView<UsingDirectiveSyntax> Usings{};
        SyntaxListView<MemberDeclarationSyntax> Members{};
        SyntaxToken* CloseBraceToken{};
        SyntaxToken* SemicolonToken{};

    public:
        explicit constexpr NamespaceDeclarationSyntax()
            : MemberDeclarationSyntax{SyntaxKind::NamespaceDeclarationSyntax}
        {
        }
    };

    class FunctionDeclarationSyntax : public MemberDeclarationSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(FunctionDeclarationSyntax);

    public:
        SyntaxListView<AttributeListSyntax> Attributes{};
        SyntaxListView<SyntaxToken> Modifiers{};
        SyntaxToken* FunctionKeyword{};
        NameSyntax* Name{};
        ParameterListSyntax* Parameters{};
        ReturnTypeClauseSyntax* ReturnType{};
        BlockStatementSyntax* Body{};
        ArrowExpressionClauseSyntax* ExpressionBody{};
        SyntaxToken* SemicolonToken{};

    public:
        explicit constexpr FunctionDeclarationSyntax()
            : MemberDeclarationSyntax{SyntaxKind::FunctionDeclarationSyntax}
        {
        }
    };

    class ReturnTypeClauseSyntax : public MemberDeclarationSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(ReturnTypeClauseSyntax);

    public:
        SyntaxToken* ArrowToken{};
        NameSyntax* Type{};

    public:
        explicit constexpr ReturnTypeClauseSyntax()
            : MemberDeclarationSyntax{SyntaxKind::ReturnTypeClauseSyntax}
        {
        }
    };

    class DelegateDeclarationSyntax : public MemberDeclarationSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(DelegateDeclarationSyntax);

    public:
    };

    class BaseTypeDeclarationSyntax : public MemberDeclarationSyntax
    {
    public:
        explicit constexpr BaseTypeDeclarationSyntax(SyntaxKind kind)
            : MemberDeclarationSyntax{kind}
        {
        }
    };

    class TypeDeclarationSyntax : public BaseTypeDeclarationSyntax
    {
    public:
        explicit constexpr TypeDeclarationSyntax(SyntaxKind kind)
            : BaseTypeDeclarationSyntax{kind}
        {
        }
    };

    class StructDeclarationSyntax : public TypeDeclarationSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(StructDeclarationSyntax);

    public:
        SyntaxListView<AttributeListSyntax> Attributes{};
        SyntaxListView<SyntaxToken> Modifiers{};
        SyntaxToken* StructKeyword{};
        NameSyntax* Name{};
        SyntaxToken* OpenBraceToken{};
        SyntaxListView<MemberDeclarationSyntax> Members{};
        SyntaxToken* CloseBraceToken{};
        SyntaxToken* SemicolonToken{};

    public:
        explicit constexpr StructDeclarationSyntax()
            : TypeDeclarationSyntax{SyntaxKind::StructDeclarationSyntax}
        {
        }
    };

    class ConceptDeclarationSyntax : public TypeDeclarationSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(ConceptDeclarationSyntax);

    public:
        SyntaxListView<AttributeListSyntax> Attributes{};
        SyntaxListView<SyntaxToken> Modifiers{};
        SyntaxToken* ConceptKeyword{};
        NameSyntax* Name{};
        SyntaxToken* OpenBraceToken{};
        SyntaxListView<MemberDeclarationSyntax> Members{};
        SyntaxToken* CloseBraceToken{};
        SyntaxToken* SemicolonToken{};

    public:
        explicit constexpr ConceptDeclarationSyntax()
            : TypeDeclarationSyntax{SyntaxKind::ConceptDeclarationSyntax}
        {
        }
    };

    class ExtendDeclarationSyntax : public TypeDeclarationSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(ExtendDeclarationSyntax);

    public:
        SyntaxListView<AttributeListSyntax> Attributes{};
        SyntaxListView<SyntaxToken> Modifiers{};
        SyntaxToken* ExtendKeyword{};
        NameSyntax* Name{};
        SyntaxToken* OpenBraceToken{};
        SyntaxListView<MemberDeclarationSyntax> Members{};
        SyntaxToken* CloseBraceToken{};
        SyntaxToken* SemicolonToken{};

    public:
        explicit constexpr ExtendDeclarationSyntax()
            : TypeDeclarationSyntax{SyntaxKind::ExtendDeclarationSyntax}
        {
        }
    };

    class FieldDeclarationSyntax : public MemberDeclarationSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(FieldDeclarationSyntax);

    public:
        SyntaxListView<AttributeListSyntax> Attributes{};
        SyntaxListView<SyntaxToken> Modifiers{};
        SyntaxToken* VarKeyword{};
        NameSyntax* Name{};
        TypeClauseSyntax* Type{};
        EqualsValueClauseSyntax* Initializer{};
        SyntaxToken* SemicolonToken{};

    public:
        explicit constexpr FieldDeclarationSyntax()
            : MemberDeclarationSyntax{SyntaxKind::FieldDeclarationSyntax}
        {
        }
    };

    class ConstantDeclarationSyntax : public MemberDeclarationSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(ConstantDeclarationSyntax);

    public:
        SyntaxListView<AttributeListSyntax> Attributes{};
        SyntaxListView<SyntaxToken> Modifiers{};
        SyntaxToken* ConstKeyword{};
        NameSyntax* Name{};
        TypeClauseSyntax* Type{};
        EqualsValueClauseSyntax* Initializer{};
        SyntaxToken* SemicolonToken{};

    public:
        explicit constexpr ConstantDeclarationSyntax()
            : MemberDeclarationSyntax{SyntaxKind::ConstantDeclarationSyntax}
        {
        }
    };

    class EnumDeclarationSyntax : public TypeDeclarationSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(EnumDeclarationSyntax);

    public:
    };

    class EnumMemberDeclarationSyntax : public MemberDeclarationSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(EnumMemberDeclarationSyntax);

    public:
    };

    class AttributeTargetSpecifierSyntax : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(AttributeTargetSpecifierSyntax);

    public:
        SyntaxToken* Identifier{};
        SyntaxToken* ColonToken{};
    };

    class AttributeSyntax : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(AttributeSyntax);

    public:
    };

    class AttributeListSyntax : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(AttributeListSyntax);

    public:
        SyntaxToken* OpenAttributeToken{};
        AttributeTargetSpecifierSyntax* Target{};
        SeparatedSyntaxListView<AttributeSyntax> Attributes{};
        SyntaxToken* CloseAttributeToken{};
    };

    class ArgumentSyntax : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(ArgumentSyntax);

    public:
        // TODO: Figure out how to handle named arguments
        SyntaxToken* DirectionKindKeyword{};
        ExpressionSyntax* Expression{};

    public:
        explicit constexpr ArgumentSyntax()
            : SyntaxNode{SyntaxKind::ArgumentSyntax}
        {
        }
    };

    class ArgumentListSyntax : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(ArgumentListSyntax);

    public:
        SyntaxToken* OpenParenToken{};
        SeparatedSyntaxListView<ArgumentSyntax> Arguments{};
        SyntaxToken* CloseParenToken{};

    public:
        explicit constexpr ArgumentListSyntax()
            : SyntaxNode{SyntaxKind::ArgumentListSyntax}
        {
        }
    };

    class BracketedArgumentListSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(BracketedArgumentListSyntax);

    public:
        SyntaxToken* OpenBracketToken{};
        SeparatedSyntaxListView<ArgumentSyntax> Arguments{};
        SyntaxToken* CloseBracketToken{};

    public:
        explicit constexpr BracketedArgumentListSyntax()
            : SyntaxNode{SyntaxKind::BracketedArgumentListSyntax}
        {
        }
    };

    class ParameterSyntax : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(ParameterSyntax);

    public:
        SyntaxListView<AttributeListSyntax> Attributes{};
        SyntaxListView<SyntaxToken> Modifiers{};
        NameSyntax* Identifier{};
        TypeClauseSyntax* Type{};

    public:
        explicit constexpr ParameterSyntax()
            : SyntaxNode{SyntaxKind::ParameterSyntax}
        {
        }
    };

    class ParameterListSyntax : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(ParameterListSyntax);

    public:
        SyntaxToken* OpenParenToken{};
        SeparatedSyntaxListView<ParameterSyntax> Parameters{};
        SyntaxToken* CloseParenToken{};

    public:
        explicit constexpr ParameterListSyntax()
            : SyntaxNode{SyntaxKind::ParameterListSyntax}
        {
        }
    };

    class GenericArgumentSyntax : public SyntaxNode
    {
    };

    class GenericArgumentListSyntax : public SyntaxNode
    {
    };

    class GenericParameterSyntax : public SyntaxNode
    {
    public:
        explicit constexpr GenericParameterSyntax(SyntaxKind kind)
            : SyntaxNode{kind}
        {
        }
    };

    class TypeGenericParameterSyntax : public GenericParameterSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(TypeGenericParameterSyntax);

    public:
        explicit constexpr TypeGenericParameterSyntax()
            : GenericParameterSyntax{SyntaxKind::TypeGenericParameterSyntax}
        {
        }
    };

    class ConstantGenericParameterSyntax : public GenericParameterSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(ConstantGenericParameterSyntax);

    public:
        explicit constexpr ConstantGenericParameterSyntax()
            : GenericParameterSyntax{SyntaxKind::ConstantGenericParameterSyntax}
        {
        }
    };

    class GenericParameterListSyntax : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(GenericParameterListSyntax);

    public:
        SyntaxToken* OpenToken{};
        SeparatedSyntaxListView<GenericParameterSyntax> Parameters{};
        SyntaxToken* CloseToken{};
    };

    class ConstraintSyntax : public SyntaxNode
    {
    };
    class ConstraintListSyntax : public SyntaxNode
    {
    };

    class ExpressionSyntax : public SyntaxNode
    {
    public:
        explicit constexpr ExpressionSyntax(SyntaxKind kind)
            : SyntaxNode{kind}
        {
        }
    };

    class InvocationExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(InvocationExpressionSyntax);

    public:
        ExpressionSyntax* Expression{};
        ArgumentListSyntax* ArgumentList{};

    public:
        explicit constexpr InvocationExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::InvocationExpressionSyntax}
        {
        }
    };

    class MemberAccessExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(MemberAccessExpressionSyntax);

    public:
        SyntaxKind OperationKind{};
        ExpressionSyntax* Expression{};
        SyntaxToken* OperatorToken{};
        SimpleNameSyntax* Name{};

    public:
        explicit constexpr MemberAccessExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::MemberAccessExpressionSyntax}
        {
        }
    };

    class ElementAccessExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(ElementAccessExpressionSyntax);

    public:
        ExpressionSyntax* Expression{};
        BracketedArgumentListSyntax* ArgumentList{};

    public:
        explicit constexpr ElementAccessExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::ElementAccessExpressionSyntax}
        {
        }
    };

    class AssignmentExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(AssignmentExpressionSyntax);

    public:
        SyntaxKind Operation{};
        ExpressionSyntax* Left{};
        SyntaxToken* OperatorToken{};
        ExpressionSyntax* Right{};

    public:
        explicit constexpr AssignmentExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::AssignmentExpressionSyntax}
        {
        }
    };

    class BinaryExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(BinaryExpressionSyntax);

    public:
        SyntaxKind Operation{};
        ExpressionSyntax* Left{};
        SyntaxToken* OperatorToken{};
        ExpressionSyntax* Right{};

    public:
        explicit constexpr BinaryExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::BinaryExpressionSyntax}
        {
        }
    };

    class UnaryExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(UnaryExpressionSyntax);

    public:
        SyntaxKind Operation{};
        SyntaxToken* OperatorToken{};
        ExpressionSyntax* Operand{};

    public:
        explicit constexpr UnaryExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::UnaryExpressionSyntax}
        {
        }
    };

    class PostfixUnaryExpression final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(PostfixUnaryExpression);

    public:
        SyntaxKind Operation{};
        ExpressionSyntax* Operand{};
        SyntaxToken* OperatorToken{};

    public:
        explicit constexpr PostfixUnaryExpression()
            : ExpressionSyntax{SyntaxKind::PostfixUnaryExpression}
        {
        }
    };

    class ParenthesizedExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(ParenthesizedExpressionSyntax);

    public:
        SyntaxToken* OpenParenToken{};
        ExpressionSyntax* Expression{};
        SyntaxToken* CloseParenToken{};

    public:
        explicit constexpr ParenthesizedExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::ParenthesizedExpressionSyntax}
        {
        }
    };

    class LiteralExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(LiteralExpressionSyntax);

    public:
        SyntaxToken* LiteralToken{};

    public:
        explicit constexpr LiteralExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::LiteralExpressionSyntax}
        {
        }
    };

    class ConditionalExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(ConditionalExpressionSyntax);

    public:
        ExpressionSyntax* Condition{};
        SyntaxToken* QuestionToken{};
        ExpressionSyntax* WhenTrue{};
        SyntaxToken* ColonToken{};
        ExpressionSyntax* WhenFalse{};

    public:
        explicit constexpr ConditionalExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::ConditionalExpressionSyntax}
        {
        }
    };

    class TypeSyntax : public ExpressionSyntax
    {
    public:
        explicit constexpr TypeSyntax(SyntaxKind kind)
            : ExpressionSyntax{kind}
        {
        }
    };

    class PredefinedTypeSyntax : public TypeSyntax
    {
        // WEAVE_DEFINE_SYNTAX_NODE(PredefinedTypeSyntax);

    public:
        SyntaxToken* Keyword{};
    };

    class NameSyntax : public TypeSyntax
    {
    public:
        explicit constexpr NameSyntax(SyntaxKind kind)
            : TypeSyntax{kind}
        {
        }
    };

    class SimpleNameSyntax : public NameSyntax
    {
    public:
        SyntaxToken* Identifier{};

    public:
        explicit constexpr SimpleNameSyntax(SyntaxKind kind)
            : NameSyntax{kind}
        {
        }
    };

    class IdentifierNameSyntax final : public SimpleNameSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(IdentifierNameSyntax);

    public:
        explicit constexpr IdentifierNameSyntax()
            : SimpleNameSyntax{SyntaxKind::IdentifierNameSyntax}
        {
        }
    };

    class GenericNameSyntax final : public SimpleNameSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(GenericNameSyntax);

    public:
        GenericArgumentListSyntax* TypeArgumentList{};

    public:
        explicit constexpr GenericNameSyntax()
            : SimpleNameSyntax{SyntaxKind::GenericNameSyntax}
        {
        }
    };

    class QualifiedNameSyntax : public NameSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(QualifiedNameSyntax);

    public:
        NameSyntax* Left{};
        SyntaxToken* DotToken{};
        SimpleNameSyntax* Right{};

    public:
        explicit constexpr QualifiedNameSyntax()
            : NameSyntax{SyntaxKind::QualifiedNameSyntax}
        {
        }
    };

    class TypeClauseSyntax : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(TypeClauseSyntax);

    public:
        SyntaxToken* ColonToken{};
        NameSyntax* Identifier{};

    public:
        explicit constexpr TypeClauseSyntax()
            : SyntaxNode{SyntaxKind::TypeClauseSyntax}
        {
        }
    };

    class EqualsValueClauseSyntax : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(EqualsValueClauseSyntax);

    public:
        SyntaxToken* EqualsToken{};
        ExpressionSyntax* Expression{};

    public:
        explicit constexpr EqualsValueClauseSyntax()
            : SyntaxNode{SyntaxKind::EqualsValueClauseSyntax}
        {
        }
    };

    class StatementSyntax : public SyntaxNode
    {
    public:
        explicit constexpr StatementSyntax(SyntaxKind kind)
            : SyntaxNode{kind}
        {
        }
    };

    class EmptyStatementSyntax final : public StatementSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(EmptyStatementSyntax);

    public:
        SyntaxToken* SemicolonToken{};

    public:
        explicit constexpr EmptyStatementSyntax()
            : StatementSyntax{SyntaxKind::EmptyStatementSyntax}
        {
        }
    };

    class VariableDeclarationSyntax : public StatementSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(VariableDeclarationSyntax);

    public:
        SyntaxToken* VarKeyword{};
        IdentifierNameSyntax* Identifier{};
        TypeClauseSyntax* TypeClause{};
        EqualsValueClauseSyntax* Initializer{};
        SyntaxToken* SemicolonToken{};

    public:
        explicit constexpr VariableDeclarationSyntax()
            : StatementSyntax{SyntaxKind::VariableDeclarationSyntax}
        {
        }
    };

    class ReturnStatementSyntax final : public StatementSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(ReturnStatementSyntax);

    public:
        SyntaxToken* ReturnKeyword{};
        ExpressionSyntax* Expression{};
        SyntaxToken* SemicolonToken{};

    public:
        explicit constexpr ReturnStatementSyntax()
            : StatementSyntax{SyntaxKind::ReturnStatementSyntax}
        {
        }
    };

    class ExpressionStatementSyntax final : public StatementSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(ExpressionStatementSyntax);

    public:
        ExpressionSyntax* Expression{};
        SyntaxToken* SemicolonToken{};

    public:
        explicit constexpr ExpressionStatementSyntax()
            : StatementSyntax{SyntaxKind::ExpressionStatementSyntax}
        {
        }
    };

    class BlockStatementSyntax final : public StatementSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(BlockStatementSyntax);

    public:
        SyntaxToken* OpenBraceToken{};
        SyntaxListView<StatementSyntax> Statements{};
        SyntaxToken* CloseBraceToken{};

    public:
        explicit constexpr BlockStatementSyntax()
            : StatementSyntax{SyntaxKind::BlockStatementSyntax}
        {
        }
    };

    class ArrowExpressionClauseSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(ArrowExpressionClauseSyntax);

    public:
        SyntaxToken* ArrowToken{};
        ExpressionSyntax* Expression{};

    public:
        explicit constexpr ArrowExpressionClauseSyntax()
            : SyntaxNode{SyntaxKind::ArrowExpressionClauseSyntax}
        {
        }
    };

    class IfStatementSyntax final : public StatementSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(IfStatementSyntax);

    public:
        SyntaxToken* IfKeyword{};
        ExpressionSyntax* Condition{};
        StatementSyntax* ThenStatement{};
        ElseClauseSyntax* ElseClause{};

    public:
        explicit constexpr IfStatementSyntax()
            : StatementSyntax{SyntaxKind::IfStatementSyntax}
        {
        }
    };

    class ElseClauseSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(ElseClauseSyntax);

    public:
        SyntaxToken* ElseKeyword{};
        BlockStatementSyntax* Statement{};

    public:
        explicit constexpr ElseClauseSyntax()
            : SyntaxNode{SyntaxKind::ElseClauseSyntax}
        {
        }
    };

    // Switch-statement
    // Case-Label
    // Default-Label
}
