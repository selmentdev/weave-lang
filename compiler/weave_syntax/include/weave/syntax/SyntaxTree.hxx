// ReSharper disable CppClangTidyClangDiagnosticExtraSemi
#pragma once
#include "weave/syntax/SyntaxNode.hxx"
#include "weave/syntax/SyntaxToken.hxx"

// Design:
//      - each syntax node should have pointer to owner tree - this way we can access the factory responsible for memory
//        allocation and deallocation.
//      - comma separated list of elements must handle separators inside element of the list
//        - for example, list of function arguments must have comma inside argument node

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

        SyntaxListView<AttributeListSyntax> Attributes{};
        SyntaxListView<SyntaxToken> Modifiers{};
        UnexpectedNodesSyntax* Unexpected{};
    };

    class MemberDeclarationSyntax : public DeclarationSyntax
    {
    public:
        explicit constexpr MemberDeclarationSyntax(SyntaxKind kind)
            : DeclarationSyntax{kind}
        {
        }
    };

    class IncompleteDeclarationSyntax : public DeclarationSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(IncompleteDeclarationSyntax);

    public:
        explicit constexpr IncompleteDeclarationSyntax()
            : DeclarationSyntax{SyntaxKind::IncompleteDeclarationSyntax}
        {
        }
    };

    class UsingDeclarationSyntax : public DeclarationSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(UsingDeclarationSyntax);

    public:
        SyntaxToken* UsingKeyword{};
        NameSyntax* Name{};

    public:
        explicit constexpr UsingDeclarationSyntax()
            : DeclarationSyntax{SyntaxKind::UsingDeclarationSyntax}
        {
        }
    };

    class CodeBlockSyntax;
    class CodeBlockItemSyntax;

    class CodeBlockSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(CodeBlockSyntax);

    public:
        explicit constexpr CodeBlockSyntax()
            : SyntaxNode{SyntaxKind::CodeBlockSyntax}
        {
        }

    public:
        UnexpectedNodesSyntax* BeforeLeftBrace{};
        SyntaxToken* LeftBrace{};
        SyntaxListView<CodeBlockItemSyntax> Elements{};
        UnexpectedNodesSyntax* BeforeRightBrace{};
        SyntaxToken* RightBrace{};
    };

    class CodeBlockItemSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(CodeBlockItemSyntax);

    public:
        explicit constexpr CodeBlockItemSyntax()
            : SyntaxNode{SyntaxKind::CodeBlockItemSyntax}
        {
        }

    public:
        SyntaxNode* Item{};
        SyntaxToken* Semicolon{};
        UnexpectedNodesSyntax* AfterSemicolon{};
    };

    class SourceFileSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(SourceFileSyntax);

    public:
        SyntaxListView<CodeBlockItemSyntax> Elements{};
        UnexpectedNodesSyntax* BeforeEndOfFileToken{};
        SyntaxToken* EndOfFileToken{};

    public:
        explicit constexpr SourceFileSyntax()
            : SyntaxNode{SyntaxKind::SourceFileSyntax}
        {
        }
    };


    class CompilationUnitSyntax : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(CompilationUnitSyntax);

    public:
        SyntaxListView<AttributeListSyntax> AttributeLists{};
        SyntaxListView<UsingDeclarationSyntax> Usings{};
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
        SyntaxToken* NamespaceKeyword{};
        NameSyntax* Name{};
        CodeBlockSyntax* Members{};

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
        SyntaxToken* FunctionKeyword{};
        NameSyntax* Name{};
        ParameterListSyntax* Parameters{};
        ReturnTypeClauseSyntax* ReturnType{};
        CodeBlockSyntax* Body{};
        ArrowExpressionClauseSyntax* ExpressionBody{};

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
        TypeSyntax* Type{};

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
        SyntaxToken* StructKeyword{};
        NameSyntax* Name{};
        CodeBlockSyntax* Members{};

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
        SyntaxToken* ConceptKeyword{};
        NameSyntax* Name{};
        CodeBlockSyntax* Members{};

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
        SyntaxToken* ExtendKeyword{};
        NameSyntax* Name{};
        CodeBlockSyntax* Members{};

    public:
        explicit constexpr ExtendDeclarationSyntax()
            : TypeDeclarationSyntax{SyntaxKind::ExtendDeclarationSyntax}
        {
        }
    };

    class ConstantDeclarationSyntax : public MemberDeclarationSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(ConstantDeclarationSyntax);

    public:
        SyntaxToken* ConstKeyword{};
        NameSyntax* Name{};
        TypeClauseSyntax* Type{};
        EqualsValueClauseSyntax* Initializer{};

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

    class BalancedTokenSequneceSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(BalancedTokenSequneceSyntax);

    public:
        explicit constexpr BalancedTokenSequneceSyntax()
            : SyntaxNode{SyntaxKind::BalancedTokenSequneceSyntax}
        {
        }

    public:
        SyntaxToken* OpenParenToken{};
        SyntaxListView<SyntaxToken> Tokens{};
        UnexpectedNodesSyntax* BeforeCloseParen{};
        SyntaxToken* CloseParenToken{};
    };

    class AttributeSyntax : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(AttributeSyntax);

    public:
        explicit constexpr AttributeSyntax()
            : SyntaxNode{SyntaxKind::AttributeSyntax}
        {
        }

    public:
        NameSyntax* Name{};
        BalancedTokenSequneceSyntax* Tokens{};
        SyntaxToken* TrailingComma{};
        // TODO: Validate if trailing comma is not present at end of attributes list.
    };

    class AttributeListSyntax : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(AttributeListSyntax);

    public:
        explicit constexpr AttributeListSyntax()
            : SyntaxNode{SyntaxKind::AttributeListSyntax}
        {
        }

    public:
        SyntaxToken* OpenAttributeToken{};
        AttributeTargetSpecifierSyntax* Target{};
        SyntaxListView<AttributeSyntax> Attributes{};
        UnexpectedNodesSyntax* BeforeCloseAttributeToken{};
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
        SyntaxListView<ArgumentSyntax> Arguments{};
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
        SyntaxListView<ArgumentSyntax> Arguments{};
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
        UnexpectedNodesSyntax* Unexpected{};
        NameSyntax* Identifier{};
        TypeClauseSyntax* Type{};
        SyntaxToken* TrailingComma{};

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
        UnexpectedNodesSyntax* BeforeOpenParenToken{};
        SyntaxToken* OpenParenToken{};
        SyntaxListView<ParameterSyntax> Parameters{};
        UnexpectedNodesSyntax* BeforeCloseParenToken{};
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
        SyntaxListView<GenericParameterSyntax> Parameters{};
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

    class PostfixUnaryExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(PostfixUnaryExpressionSyntax);

    public:
        SyntaxKind Operation{};
        ExpressionSyntax* Operand{};
        SyntaxToken* OperatorToken{};

    public:
        explicit constexpr PostfixUnaryExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::PostfixUnaryExpressionSyntax}
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

    class SelfExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(SelfExpressionSyntax);

    public:
        SyntaxToken* SelfKeyword{};

    public:
        explicit constexpr SelfExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::SelfExpressionSyntax}
        {
        }
    };

    class SizeOfExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(SizeOfExpressionSyntax);

    public:
        SyntaxToken* SizeOfKeyword{};
        SyntaxToken* OpenParenToken{};
        ExpressionSyntax* Expression{};
        SyntaxToken* CloseParenToken{};

    public:
        explicit constexpr SizeOfExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::SizeOfExpressionSyntax}
        {
        }
    };

    class AlignOfExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(AlignOfExpressionSyntax);

    public:
        SyntaxToken* AlignOfKeyword{};
        SyntaxToken* OpenParenToken{};
        ExpressionSyntax* Expression{};
        SyntaxToken* CloseParenToken{};

    public:
        explicit constexpr AlignOfExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::AlignOfExpressionSyntax}
        {
        }
    };

    class TypeOfExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(TypeOfExpressionSyntax);

    public:
        SyntaxToken* TypeOfKeyword{};
        SyntaxToken* OpenParenToken{};
        ExpressionSyntax* Expression{};
        SyntaxToken* CloseParenToken{};

    public:
        explicit constexpr TypeOfExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::TypeOfExpressionSyntax}
        {
        }
    };

    class NameOfExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(NameOfExpressionSyntax);

    public:
        SyntaxToken* NameOfKeyword{};
        SyntaxToken* OpenParenToken{};
        ExpressionSyntax* Expression{};
        SyntaxToken* CloseParenToken{};

    public:
        explicit constexpr NameOfExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::NameOfExpressionSyntax}
        {
        }
    };

    class AddressOfExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(AddressOfExpressionSyntax);

    public:
        SyntaxToken* AddressOfKeyword{};
        SyntaxToken* OpenParenToken{};
        ExpressionSyntax* Expression{};
        SyntaxToken* CloseParenToken{};

    public:
        explicit constexpr AddressOfExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::AddressOfExpressionSyntax}
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

    class TupleTypeSyntax final : public TypeSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(TupleTypeSyntax);

    public:
        UnexpectedNodesSyntax* BeforeOpenParenToken{};
        SyntaxToken* OpenParenToken{};
        SyntaxListView<TupleTypeElementSyntax> Elements{};
        UnexpectedNodesSyntax* BeforeCloseParenToken{};
        SyntaxToken* CloseParenToken{};

    public:
        explicit constexpr TupleTypeSyntax()
            : TypeSyntax{SyntaxKind::TupleTypeSyntax}
        {
        }
    };

    class TupleTypeElementSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(TupleTypeElementSyntax);

    public:
        SyntaxToken* Name{};
        SyntaxToken* Colon{};
        TypeSyntax* Type{};
        SyntaxToken* TrailingComma{};

    public:
        explicit constexpr TupleTypeElementSyntax()
            : SyntaxNode{SyntaxKind::TupleTypeElementSyntax}
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

    class TupleIndexSyntax final : public SimpleNameSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(TupleIndexSyntax);

    public:
        explicit constexpr TupleIndexSyntax()
            : SimpleNameSyntax{SyntaxKind::TupleIndexSyntax}
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
        TypeSyntax* Type{};

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

    public:
        SyntaxListView<AttributeListSyntax> Attributes{};
        SyntaxListView<SyntaxToken> Modifiers{};
        UnexpectedNodesSyntax* Unexpected{};
    };

    class EmptyStatementSyntax final : public StatementSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(EmptyStatementSyntax);

    public:
        explicit constexpr EmptyStatementSyntax()
            : StatementSyntax{SyntaxKind::EmptyStatementSyntax}
        {
        }
    };

    class VariableDeclarationSyntax : public DeclarationSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(VariableDeclarationSyntax);

    public:
        SyntaxToken* VarKeyword{};
        IdentifierNameSyntax* Identifier{};
        TypeClauseSyntax* TypeClause{};
        EqualsValueClauseSyntax* Initializer{};

    public:
        explicit constexpr VariableDeclarationSyntax()
            : DeclarationSyntax{SyntaxKind::VariableDeclarationSyntax}
        {
        }
    };

    class ReturnStatementSyntax final : public StatementSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(ReturnStatementSyntax);

    public:
        SyntaxToken* ReturnKeyword{};
        ExpressionSyntax* Expression{};

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
        CodeBlockSyntax* Members{};

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
        SyntaxListView<AttributeListSyntax> ConditionAttributes{};
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
        StatementSyntax* Statement{};

    public:
        explicit constexpr ElseClauseSyntax()
            : SyntaxNode{SyntaxKind::ElseClauseSyntax}
        {
        }
    };

    class WhileStatementSyntax final : public StatementSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(WhileStatementSyntax);

    public:
        SyntaxToken* WhileKeyword{};
        SyntaxListView<AttributeListSyntax> ConditionAttributes{};
        ExpressionSyntax* Condition{};
        StatementSyntax* Statement{};

    public:
        explicit constexpr WhileStatementSyntax()
            : StatementSyntax{SyntaxKind::WhileStatementSyntax}
        {
        }
    };

    class BreakStatementSyntax final : public StatementSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(BreakStatementSyntax);

    public:
        SyntaxToken* BreakKeyword{};
        SyntaxToken* Label{};

    public:
        explicit constexpr BreakStatementSyntax()
            : StatementSyntax{SyntaxKind::BreakStatementSyntax}
        {
        }
    };

    class ContinueStatementSyntax final : public StatementSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(ContinueStatementSyntax);

    public:
        SyntaxToken* ContinueKeyword{};

    public:
        explicit constexpr ContinueStatementSyntax()
            : StatementSyntax{SyntaxKind::ContinueStatementSyntax}
        {
        }
    };

    class GotoStatementSyntax final : public StatementSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(GotoStatementSyntax);

    public:
        SyntaxToken* GotoKeyword{};
        SyntaxToken* Label{};

    public:
        explicit constexpr GotoStatementSyntax()
            : StatementSyntax{SyntaxKind::GotoStatementSyntax}
        {
        }
    };

    class UnexpectedNodesSyntax : public SyntaxNode
    {
    public:
        static constexpr bool ClassOf(SyntaxKind kind)
        {
            return kind == SyntaxKind::UnexpectedNodesSyntax;
        }

        static constexpr bool ClassOf(SyntaxNode const* node)
        {
            return node->Is(SyntaxKind::UnexpectedNodesSyntax);
        }

    public:
        SyntaxListView<SyntaxNode> Nodes{};

        explicit constexpr UnexpectedNodesSyntax()
            : SyntaxNode{SyntaxKind::UnexpectedNodesSyntax}
        {
        }
    };

    class LabelStatementSyntax final : public StatementSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(LabelStatementSyntax);

    public:
        SyntaxToken* Name{};
        SyntaxToken* Colon{};
        StatementSyntax* Statement{};

    public:
        explicit constexpr LabelStatementSyntax()
            : StatementSyntax{SyntaxKind::LabelStatementSyntax}
        {
        }
    };

    class TupleExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(TupleExpressionSyntax);

    public:
        UnexpectedNodesSyntax* BeforeOpenParenToken{};
        SyntaxToken* OpenParenToken{};
        SyntaxListView<LabeledExpressionSyntax> Elements{};
        UnexpectedNodesSyntax* BeforeCloseParenToken{};
        SyntaxToken* CloseParenToken{};

    public:
        explicit constexpr TupleExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::TupleExpressionSyntax}
        {
        }
    };

    class LabeledExpressionSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(LabeledExpressionSyntax);

    public:
        SyntaxToken* Label{};
        SyntaxToken* Colon{};
        ExpressionSyntax* Expression{};
        SyntaxToken* TrailingComma{};

    public:
        explicit constexpr LabeledExpressionSyntax()
            : SyntaxNode{SyntaxKind::LabeledExpressionSyntax}
        {
        }
    };
}
