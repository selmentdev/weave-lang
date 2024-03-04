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
    };

    class MemberDeclarationSyntax : public DeclarationSyntax
    {
    public:
        explicit constexpr MemberDeclarationSyntax(SyntaxKind kind)
            : DeclarationSyntax{kind}
        {
        }
    };

    class IncompleteDeclarationSyntax final : public DeclarationSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(IncompleteDeclarationSyntax);

    public:
        explicit constexpr IncompleteDeclarationSyntax()
            : DeclarationSyntax{SyntaxKind::IncompleteDeclarationSyntax}
        {
        }
    };

    class UsingDeclarationSyntax final : public DeclarationSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(UsingDeclarationSyntax);

    public:
        SyntaxToken* UsingKeyword{};
        PathSyntax* Name{};

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

    class NamespaceDeclarationSyntax final : public MemberDeclarationSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(NamespaceDeclarationSyntax);

    public:
        SyntaxToken* NamespaceKeyword{};
        PathSyntax* Name{};
        CodeBlockSyntax* Members{};

    public:
        explicit constexpr NamespaceDeclarationSyntax()
            : MemberDeclarationSyntax{SyntaxKind::NamespaceDeclarationSyntax}
        {
        }
    };

    class FunctionDeclarationSyntax final : public MemberDeclarationSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(FunctionDeclarationSyntax);

    public:
        SyntaxToken* FunctionKeyword{};
        IdentifierSyntax* Name{};
        GenericParametersSyntax* GenericParameters{};
        ParameterListSyntax* Parameters{};
        ReturnTypeClauseSyntax* ReturnType{};
        SyntaxListView<ConstraintClauseSyntax> Constraints{};
        UnexpectedNodesSyntax* BeforeBody{};
        CodeBlockSyntax* Body{};
        ArrowExpressionClauseSyntax* ExpressionBody{};

    public:
        explicit constexpr FunctionDeclarationSyntax()
            : MemberDeclarationSyntax{SyntaxKind::FunctionDeclarationSyntax}
        {
        }
    };

    class ReturnTypeClauseSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(ReturnTypeClauseSyntax);

    public:
        SyntaxToken* ArrowToken{};

        SyntaxListView<SyntaxToken> Specifiers{};

        NameColonSyntax* Name{};

        TypeSyntax* Type{};

    public:
        explicit constexpr ReturnTypeClauseSyntax()
            : SyntaxNode{SyntaxKind::ReturnTypeClauseSyntax}
        {
        }
    };

    class DelegateDeclarationSyntax final : public MemberDeclarationSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(DelegateDeclarationSyntax);

    public:
        SyntaxToken* DelegateKeyword{};
        IdentifierSyntax* Name{};
        GenericParametersSyntax* GenericParameters{};
        ParameterListSyntax* Parameters{};
        ReturnTypeClauseSyntax* ReturnType{};
        SyntaxListView<ConstraintClauseSyntax> Constraints{};

    public:
        explicit constexpr DelegateDeclarationSyntax()
            : MemberDeclarationSyntax{SyntaxKind::DelegateDeclarationSyntax}
        {
        }
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

    class StructDeclarationSyntax final : public TypeDeclarationSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(StructDeclarationSyntax);

    public:
        SyntaxToken* StructKeyword{};
        IdentifierSyntax* Name{};
        GenericParametersSyntax* GenericParameters{};
        SyntaxListView<ConstraintClauseSyntax> Constraints{};
        CodeBlockSyntax* Members{};

    public:
        explicit constexpr StructDeclarationSyntax()
            : TypeDeclarationSyntax{SyntaxKind::StructDeclarationSyntax}
        {
        }
    };

    class UnionDeclarationSyntax final : public TypeDeclarationSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(UnionDeclarationSyntax);

    public:
        SyntaxToken* UnionKeyword{};
        IdentifierSyntax* Name{};
        GenericParametersSyntax* GenericParameters{};
        SyntaxListView<ConstraintClauseSyntax> Constraints{};
        CodeBlockSyntax* Members{};

    public:
        explicit constexpr UnionDeclarationSyntax()
            : TypeDeclarationSyntax{SyntaxKind::UnionDeclarationSyntax}
        {
        }
    };

    class ConceptDeclarationSyntax final : public TypeDeclarationSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(ConceptDeclarationSyntax);

    public:
        SyntaxToken* ConceptKeyword{};
        IdentifierSyntax* Name{};
        GenericParametersSyntax* GenericParameters{};
        SyntaxListView<ConstraintClauseSyntax> Constraints{};
        CodeBlockSyntax* Members{};

    public:
        explicit constexpr ConceptDeclarationSyntax()
            : TypeDeclarationSyntax{SyntaxKind::ConceptDeclarationSyntax}
        {
        }
    };

    class ExtendDeclarationSyntax final : public TypeDeclarationSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(ExtendDeclarationSyntax);

    public:
        SyntaxToken* ExtendKeyword{};
        IdentifierSyntax* Name{};
        GenericParametersSyntax* GenericParameters{};
        CodeBlockSyntax* Members{};
        SyntaxToken* AsKeyword{};
        TypeSyntax* ConceptType{};
        SyntaxListView<ConstraintClauseSyntax> Constraints{};

    public:
        explicit constexpr ExtendDeclarationSyntax()
            : TypeDeclarationSyntax{SyntaxKind::ExtendDeclarationSyntax}
        {
        }
    };

    class TypeAliasDeclarationSyntax final : public TypeDeclarationSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(TypeAliasDeclarationSyntax);

    public:
        SyntaxToken* TypeKeyword{};
        IdentifierSyntax* Name{};
        SyntaxToken* EqualsToken{};
        TypeSyntax* Type{};

    public:
        explicit constexpr TypeAliasDeclarationSyntax()
            : TypeDeclarationSyntax{SyntaxKind::TypeAliasDeclarationSyntax}
        {
        }
    };
    
    class ConstantDeclarationSyntax final : public MemberDeclarationSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(ConstantDeclarationSyntax);

    public:
        SyntaxToken* ConstKeyword{};
        IdentifierSyntax* Name{};
        TypeClauseSyntax* Type{};
        ExpressionInitializerClauseSyntax* Initializer{};

    public:
        explicit constexpr ConstantDeclarationSyntax()
            : MemberDeclarationSyntax{SyntaxKind::ConstantDeclarationSyntax}
        {
        }
    };

    class EnumDeclarationSyntax final : public TypeDeclarationSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(EnumDeclarationSyntax);

    public:
        SyntaxToken* EnumKeyword{};
        IdentifierSyntax* Name{};
        GenericParametersSyntax* GenericParameters{};
        TypeInheritanceClause* BaseType{};

        UnexpectedNodesSyntax* BeforeOpenBrace{};
        SyntaxToken* OpenBraceToken{};

        SyntaxListView<EnumMemberDeclarationSyntax> Members{};

        UnexpectedNodesSyntax* BeforeCloseBrace{};
        SyntaxToken* CloseBraceToken{};

    public:
        explicit constexpr EnumDeclarationSyntax()
            : TypeDeclarationSyntax{SyntaxKind::EnumDeclarationSyntax}
        {
        }
    };

    class EnumMemberDeclarationSyntax final : public DeclarationSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(EnumMemberDeclarationSyntax);

    public:
        IdentifierSyntax* Identifier{};
        TupleTypeSyntax* Tuple{};
        ExpressionInitializerClauseSyntax* Discriminator{};
        SyntaxToken* TrailingComma{};

    public:
        explicit constexpr EnumMemberDeclarationSyntax()
            : DeclarationSyntax{SyntaxKind::EnumMemberDeclarationSyntax}
        {
        }
    };

    class AttributeTargetSpecifierSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(AttributeTargetSpecifierSyntax);

    public:
        SyntaxToken* Identifier{};
        SyntaxToken* ColonToken{};
    };

    class BalancedTokenSequenceSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(BalancedTokenSequenceSyntax);

    public:
        explicit constexpr BalancedTokenSequenceSyntax()
            : SyntaxNode{SyntaxKind::BalancedTokenSequenceSyntax}
        {
        }

    public:
        SyntaxToken* OpenParenToken{};
        SyntaxListView<SyntaxToken> Tokens{};
        UnexpectedNodesSyntax* BeforeCloseParen{};
        SyntaxToken* CloseParenToken{};
    };

    /// attributes
    ///     : attribute-section+
    ///     ;
    ///
    /// attribute-section
    ///     : '#[' attribute-target-specifier? attribute-sequence ','? ']'
    ///     ;
    ///
    /// attribute-sequence
    ///     : attribute (',' attribute)*
    ///     ;
    ///
    /// attribute-target-specifier
    ///     : attribute-target ':'
    ///     ;
    ///
    /// attribute-target
    ///     : identifier
    ///     | keyword
    ///     ;

    /// attribute
    ///     : 'name' balanced-token-sequence?
    ///     ;
    class AttributeSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(AttributeSyntax);

    public:
        explicit constexpr AttributeSyntax()
            : SyntaxNode{SyntaxKind::AttributeSyntax}
        {
        }

    public:
        PathSyntax* Name{};
        BalancedTokenSequenceSyntax* Tokens{};
        SyntaxToken* TrailingComma{};
        // TODO: Validate if trailing comma is not present at end of attributes list.
    };

    /// attribute-sequence
    ///     : attribute (',' attribute)*
    ///     ;
    ///
    /// attribute-list
    ///     : '#[' attribute-target-specifier? attribute-sequence ']'
    ///     ;
    class AttributeListSyntax final : public SyntaxNode
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

    class ArgumentSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(ArgumentSyntax);

    public:
        SyntaxListView<AttributeListSyntax> Attributes{};
        SyntaxListView<SyntaxToken> Modifiers{};

        NameColonSyntax* Name{};
        ExpressionSyntax* Expression{};
        SyntaxToken* TrailingComma{};

    public:
        explicit constexpr ArgumentSyntax()
            : SyntaxNode{SyntaxKind::ArgumentSyntax}
        {
        }
    };

    class ArgumentListSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(ArgumentListSyntax);

    public:
        UnexpectedNodesSyntax* BeforeOpenParenToken{};
        SyntaxToken* OpenParenToken{};
        SyntaxListView<ArgumentSyntax> Arguments{};
        UnexpectedNodesSyntax* BeforeCloseParenToken{};
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
        UnexpectedNodesSyntax* BeforeOpenBracketToken{};
        SyntaxToken* OpenBracketToken{};
        SyntaxListView<ArgumentSyntax> Arguments{};
        UnexpectedNodesSyntax* BeforeCloseBracketToken{};
        SyntaxToken* CloseBracketToken{};

    public:
        explicit constexpr BracketedArgumentListSyntax()
            : SyntaxNode{SyntaxKind::BracketedArgumentListSyntax}
        {
        }
    };

    class ParameterSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(ParameterSyntax);

    public:
        SyntaxListView<AttributeListSyntax> Attributes{};
        SyntaxListView<SyntaxToken> Modifiers{};
        IdentifierSyntax* Identifier{};
        TypeClauseSyntax* Type{};
        SyntaxToken* TrailingComma{};

    public:
        explicit constexpr ParameterSyntax()
            : SyntaxNode{SyntaxKind::ParameterSyntax}
        {
        }
    };

    class ParameterListSyntax final : public SyntaxNode
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

    class GenericArgumentSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(GenericArgumentSyntax)

    public:
        ExpressionSyntax* Expression{}; // Either type or value
        SyntaxToken* TrailingComma{};

    public:
        explicit constexpr GenericArgumentSyntax()
            : SyntaxNode{SyntaxKind::GenericArgumentSyntax}
        {
        }
    };

    class GenericArgumentsSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(GenericArgumentsSyntax);

    public:
        UnexpectedNodesSyntax* BeforeOpenToken{};
        SyntaxToken* OpenToken{};
        SyntaxListView<GenericArgumentSyntax> Arguments{};
        UnexpectedNodesSyntax* BeforeCloseToken{};
        SyntaxToken* CloseToken{};

    public:
        explicit constexpr GenericArgumentsSyntax()
            : SyntaxNode{SyntaxKind::GenericArgumentsSyntax}
        {
        }
    };


    // Generic parameters
    class GenericParameterSyntax : public SyntaxNode
    {
    public:
        explicit constexpr GenericParameterSyntax(SyntaxKind kind)
            : SyntaxNode{kind}
        {
        }
    };

    // type-generic-parameter
    //      : `type' name ':' requirements '=' expression
    //      ;
    class TypeGenericParameterSyntax final : public GenericParameterSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(TypeGenericParameterSyntax);

    public:
        SyntaxToken* TypeKeyword{};
        IdentifierSyntax* Name{};

        TypeInitializerClauseSyntax* Initializer{};

        SyntaxToken* TrailingComma{};

    public:
        explicit constexpr TypeGenericParameterSyntax()
            : GenericParameterSyntax{SyntaxKind::TypeGenericParameterSyntax}
        {
        }
    };

    // const-generic-parameter
    //      : 'const' name ':' type '=' expression
    //      ;
    class ConstGenericParameterSyntax final : public GenericParameterSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(ConstGenericParameterSyntax);

    public:
        SyntaxToken* ConstKeyword{};
        IdentifierSyntax* Name{};

        TypeClauseSyntax* Type{};
        ExpressionInitializerClauseSyntax* Initializer{};

        SyntaxToken* TrailingComma{};

    public:
        explicit constexpr ConstGenericParameterSyntax()
            : GenericParameterSyntax{SyntaxKind::ConstGenericParameterSyntax}
        {
        }
    };

    // generic-parameter
    //      : type-generic-parameter
    //      | const-generic-parameter
    //      ;
    //
    // generic-parameter-list
    //      : generic-parameter (',' generic-parameter)*
    //      ;
    // generic-parameters
    //      : '::<' generic-parameter-list? '>'
    //      ;
    class GenericParametersSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(GenericParametersSyntax);

    public:
        UnexpectedNodesSyntax* BeforeOpenToken{};
        SyntaxToken* OpenToken{};
        SyntaxListView<GenericParameterSyntax> Parameters{};
        UnexpectedNodesSyntax* BeforeCloseToken{};
        SyntaxToken* CloseToken{};

    public:
        explicit constexpr GenericParametersSyntax()
            : SyntaxNode{SyntaxKind::GenericParametersSyntax}
        {
        }
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
        NameSyntax* Name{};

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

    class UnreachableExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(UnreachableExpressionSyntax);

    public:
        SyntaxNode* UnreachableKeyword{};

    public:
        explicit constexpr UnreachableExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::UnreachableExpressionSyntax}
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

    class OldExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(OldExpressionSyntax);

    public:
        SyntaxToken* OldKeyword{};
        UnexpectedNodesSyntax* BeforeOpenParenToken{};
        SyntaxToken* OpenParenToken{};
        ExpressionSyntax* Expression{};
        UnexpectedNodesSyntax* BeforeCloseParenToken{};
        SyntaxToken* CloseParenToken{};

    public:
        explicit constexpr OldExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::OldExpressionSyntax}
        {
        }
    };

    class RefExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(RefExpressionSyntax);

    public:
        SyntaxToken* RefKeyword{};
        ExpressionSyntax* Expression{};

    public:
        explicit constexpr RefExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::RefExpressionSyntax}
        {
        }
    };

    class MoveExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(MoveExpressionSyntax);

    public:
        SyntaxToken* MoveKeyword{};
        ExpressionSyntax* Expression{};

    public:
        explicit constexpr MoveExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::MoveExpressionSyntax}
        {
        }
    };

    class OutExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(OutExpressionSyntax);

    public:
        SyntaxToken* OutKeyword{};
        ExpressionSyntax* Expression{};

    public:
        explicit constexpr OutExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::OutExpressionSyntax}
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

    class TypePointerSyntax final : public TypeSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(TypePointerSyntax);

    public:
        SyntaxToken* AsteriskToken{};
        SyntaxListView<SyntaxToken> Qualifiers{};
        TypeSyntax* Type{};

    public:
        explicit constexpr TypePointerSyntax()
            : TypeSyntax{SyntaxKind::TypePointerSyntax}
        {
        }
    };

    class TypeReferenceSyntax final : public TypeSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(TypeReferenceSyntax);

    public:
        SyntaxToken* AmpersandToken{};
        SyntaxListView<SyntaxToken> Qualifiers{};
        TypeSyntax* Type{};

    public:
        explicit constexpr TypeReferenceSyntax()
            : TypeSyntax{SyntaxKind::TypeReferenceSyntax}
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
        NameColonSyntax* Name{};
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

    class ArrayTypeSyntax final : public TypeSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(ArrayTypeSyntax);

    public:
        SyntaxToken* OpenBracketToken{};
        TypeSyntax* ElementType{};
        SyntaxToken* ColonToken{};
        ExpressionSyntax* LengthExpression{};
        SyntaxToken* CloseBracketToken{};

    public:
        explicit constexpr ArrayTypeSyntax()
            : TypeSyntax{SyntaxKind::ArrayTypeSyntax}
        {
        }
    };

    class SliceTypeSyntax final : public TypeSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(SliceTypeSyntax);

    public:
        SyntaxToken* OpenBracketToken{};
        TypeSyntax* ElementType{};
        SyntaxToken* CloseBracketToken{};

    public:
        explicit constexpr SliceTypeSyntax()
            : TypeSyntax{SyntaxKind::SliceTypeSyntax}
        {
        }
    };

    class TypeClauseSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(TypeClauseSyntax);

    public:
        SyntaxToken* ColonToken{};

        SyntaxListView<SyntaxToken> Specifiers{};
        TypeSyntax* Type{};

    public:
        explicit constexpr TypeClauseSyntax()
            : SyntaxNode{SyntaxKind::TypeClauseSyntax}
        {
        }
    };

    class ExpressionInitializerClauseSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(ExpressionInitializerClauseSyntax);

    public:
        SyntaxToken* EqualsToken{};
        ExpressionSyntax* Expression{};

    public:
        explicit constexpr ExpressionInitializerClauseSyntax()
            : SyntaxNode{SyntaxKind::ExpressionInitializerClauseSyntax}
        {
        }
    };

    class TypeInitializerClauseSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(TypeInitializerClauseSyntax);

    public:
        SyntaxToken* EqualsToken{};
        TypeSyntax* DefaultType{};

    public:
        explicit constexpr TypeInitializerClauseSyntax()
            : SyntaxNode{SyntaxKind::TypeInitializerClauseSyntax}
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

    class VariableDeclarationSyntax final : public DeclarationSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(VariableDeclarationSyntax);

    public:
        SyntaxToken* BindingSpecifier{};
        PatternSyntax* Pattern{};
        ExpressionInitializerClauseSyntax* Initializer{};

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

    class IfExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(IfExpressionSyntax);

    public:
        SyntaxToken* IfKeyword{};
        SyntaxListView<AttributeListSyntax> ConditionAttributes{};
        SyntaxToken* OpenParenToken{};
        ExpressionSyntax* Condition{};
        SyntaxToken* CloseParenToken{};
        CodeBlockSyntax* Body{};
        ElseClauseSyntax* ElseClause{};

    public:
        explicit constexpr IfExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::IfExpressionSyntax}
        {
        }
    };

    class ElseClauseSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(ElseClauseSyntax);

    public:
        SyntaxToken* ElseKeyword{};
        IfExpressionSyntax* Continuation{};
        CodeBlockSyntax* Body{};

    public:
        explicit constexpr ElseClauseSyntax()
            : SyntaxNode{SyntaxKind::ElseClauseSyntax}
        {
        }
    };

    class WithExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(WithExpressionSyntax);

    public:
        ExpressionSyntax* Expression{};
        SyntaxToken* WithKeyword{};

        UnexpectedNodesSyntax* BeforeOpenBraceToken{};
        SyntaxToken* OpenBraceToken{};
        SyntaxListView<LabeledExpressionSyntax> Elements{};
        UnexpectedNodesSyntax* BeforeCloseBraceToken{};
        SyntaxToken* CloseBraceToken{};

    public:
        explicit constexpr WithExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::WithExpressionSyntax}
        {
        }
    };

    class WhileStatementSyntax final : public StatementSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(WhileStatementSyntax);

    public:
        SyntaxToken* WhileKeyword{};
        SyntaxListView<AttributeListSyntax> ConditionAttributes{};
        SyntaxToken* OpenParenToken{};
        ExpressionSyntax* Condition{};
        SyntaxToken* CloseParenToken{};
        CodeBlockSyntax* Body{};

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

    class LoopStatementSyntax final : public StatementSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(LoopStatementSyntax);

    public:
        SyntaxToken* LoopKeyword{};
        CodeBlockSyntax* Body{};

    public:
        explicit constexpr LoopStatementSyntax()
            : StatementSyntax{SyntaxKind::LoopStatementSyntax}
        {
        }
    };

    class RepeatStatementSyntax final : public StatementSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(RepeatStatementSyntax);

    public:
        SyntaxToken* RepeatKeyword{};
        CodeBlockSyntax* Body{};
        SyntaxToken* WhileKeyword{};
        SyntaxListView<AttributeListSyntax> ConditionAttributes{};
        SyntaxToken* OpenParenToken{};
        ExpressionSyntax* Condition{};
        SyntaxToken* CloseParenToken{};

    public:
        explicit constexpr RepeatStatementSyntax()
            : StatementSyntax{SyntaxKind::RepeatStatementSyntax}
        {
        }
    };

    class CheckedStatementSyntax final : public StatementSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(CheckedStatementSyntax);

    public:
        SyntaxToken* CheckedKeyword{};
        CodeBlockSyntax* Body{};

    public:
        explicit constexpr CheckedStatementSyntax()
            : StatementSyntax{SyntaxKind::CheckedStatementSyntax}
        {
        }
    };

    class UncheckedStatementSyntax final : public StatementSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(UncheckedStatementSyntax);

    public:
        SyntaxToken* UncheckedKeyword{};
        CodeBlockSyntax* Body{};

    public:
        explicit constexpr UncheckedStatementSyntax()
            : StatementSyntax{SyntaxKind::UncheckedStatementSyntax}
        {
        }
    };

    class UnsafeStatementSyntax final : public StatementSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(UnsafeStatementSyntax);

    public:
        SyntaxToken* UnsafeKeyword{};
        CodeBlockSyntax* Body{};

    public:
        explicit constexpr UnsafeStatementSyntax()
            : StatementSyntax{SyntaxKind::UnsafeStatementSyntax}
        {
        }
    };

    class LazyStatementSyntax final : public StatementSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(LazyStatementSyntax);

    public:
        SyntaxToken* LazyKeyword{};
        CodeBlockSyntax* Body{};

    public:
        explicit constexpr LazyStatementSyntax()
            : StatementSyntax{SyntaxKind::LazyStatementSyntax}
        {
        }
    };

    class AssertExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(AssertExpressionSyntax);

    public:
        SyntaxToken* AssertKeyword{};

        UnexpectedNodesSyntax* BeforeOpenParenToken{};
        SyntaxToken* OpenParenToken{};

        NameColonSyntax* Level{};

        ExpressionSyntax* Condition{};

        UnexpectedNodesSyntax* BeforeCloseParenToken{};
        SyntaxToken* CloseParenToken{};

    public:
        explicit constexpr AssertExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::AssertExpressionSyntax}
        {
        }
    };

    /// unexpected-nodes
    ///     : syntax-token*
    ///     ;
    class UnexpectedNodesSyntax final : public SyntaxNode
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

    /// labeled-statement
    ///     : name-colon? statement
    ///     ;
    class LabeledStatementSyntax final : public StatementSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(LabeledStatementSyntax);

    public:
        NameColonSyntax* Name{};
        StatementSyntax* Statement{};

    public:
        explicit constexpr LabeledStatementSyntax()
            : StatementSyntax{SyntaxKind::LabeledStatementSyntax}
        {
        }
    };

    /// tuple-expression
    ///     : '(' labeled-expression-sequence? ')'
    ///     ;
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

    /// struct-expression
    ///     : name brace-initializer-clause
    ///     ;
    class StructExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(StructExpressionSyntax);

    public:
        TypePathSyntax* TypeName{};
        UnexpectedNodesSyntax* BeforeOpenBraceToken{};
        SyntaxToken* OpenBraceToken{};
        SyntaxListView<LabeledExpressionSyntax> Elements{};
        UnexpectedNodesSyntax* BeforeCloseBraceToken{};
        SyntaxToken* CloseBraceToken{};

    public:
        explicit constexpr StructExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::StructExpressionSyntax}
        {
        }
    };

    class ArrayExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(ArrayExpressionSyntax);

    public:
        UnexpectedNodesSyntax* BeforeOpenBracketToken{};
        SyntaxToken* OpenBracketToken{};
        SyntaxListView<LabeledExpressionSyntax> Elements{};
        UnexpectedNodesSyntax* BeforeCloseBracketToken{};
        SyntaxToken* CloseBracketToken{};

    public:
        explicit constexpr ArrayExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::ArrayExpressionSyntax}
        {
        }
    };

    /// labeled-expression
    ///     : name-colon? expression
    ///     ;
    ///
    /// labeled-expression-sequence
    ///     : labeled-expression (',' labeled-expression)*
    ///     ;
    class LabeledExpressionSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(LabeledExpressionSyntax);

    public:
        NameColonSyntax* Name{};
        ExpressionSyntax* Expression{};
        SyntaxToken* TrailingComma{};

    public:
        explicit constexpr LabeledExpressionSyntax()
            : SyntaxNode{SyntaxKind::LabeledExpressionSyntax}
        {
        }
    };

    class ConstraintClauseSyntax : public SyntaxNode
    {
    public:
        explicit constexpr ConstraintClauseSyntax(SyntaxKind kind)
            : SyntaxNode{kind}
        {
        }
    };

    /// contract-kind
    ///     : 'requires' | 'ensures' | 'invariant' | 'assert'
    ///     ;
    ///
    /// contract-clause
    ///     : contract-kind '(' identifier ':' expression ')'
    ///     | contract-kind '(' expression ')'
    ///     ;

    class ContractClauseSyntax final : public ConstraintClauseSyntax
    {
    public:
        SyntaxToken* Introducer{};

        UnexpectedNodesSyntax* BeforeOpenParenToken{};
        SyntaxToken* OpenParenToken{};

        NameColonSyntax* Level{};

        ExpressionSyntax* Condition{};

        UnexpectedNodesSyntax* BeforeCloseParenToken{};
        SyntaxToken* CloseParenToken{};

    public:
        explicit constexpr ContractClauseSyntax()
            : ConstraintClauseSyntax{SyntaxKind::ContractClauseSyntax}
        {
        }
    };

    class WherePredicateSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(WherePredicateSyntax);

    public:
        TypePathSyntax* Type{};
        SyntaxToken* TrailingComma{};

    public:
        explicit constexpr WherePredicateSyntax()
            : SyntaxNode{SyntaxKind::WherePredicateSyntax}
        {
        }
    };

    /// where-predicate
    ///     : type
    ///     ;
    ///
    /// where-predicate-sequence
    ///     : where-predicate (',' where-predicate)*
    ///     ;
    ///
    /// where-clause
    ///     : 'where' '(' type ':' where-predicate-sequence ')'
    ///     ;

    class WhereClauseSyntax final : public ConstraintClauseSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(WhereClauseSyntax);

    public:
        SyntaxToken* WhereKeyword{};

        UnexpectedNodesSyntax* BeforeOpenParenToken{};
        SyntaxToken* OpenParenToken{};

        IdentifierSyntax* Type{};

        UnexpectedNodesSyntax* BeforeColon{};
        SyntaxToken* Colon{};

        SyntaxListView<WherePredicateSyntax> Predicates{};

        UnexpectedNodesSyntax* BeforeCloseParenToken{};
        SyntaxToken* CloseParenToken{};

    public:
        explicit constexpr WhereClauseSyntax()
            : ConstraintClauseSyntax{SyntaxKind::WhereClauseSyntax}
        {
        }
    };

    /// yield-statement
    ///     : 'yield' expression
    ///     | 'yield' 'break'
    ///     | 'yield' 'return' expression
    ///     ;
    class YieldStatementSyntax final : public StatementSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(YieldStatementSyntax);

    public:
        SyntaxToken* YieldKeyword{};
        SyntaxToken* KindKeyword{};
        ExpressionSyntax* Expression{};

    public:
        explicit constexpr YieldStatementSyntax()
            : StatementSyntax{SyntaxKind::YieldStatementSyntax}
        {
        }
    };

    /// eval-expression
    ///     : 'eval' code-block-item
    ///     ;
    class EvalExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(EvalExpressionSyntax);

    public:
        SyntaxToken* EvalKeyword{};
        CodeBlockItemSyntax* Body{};

    public:
        explicit constexpr EvalExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::EvalExpressionSyntax}
        {
        }
    };

    class MatchClauseSyntax : public SyntaxNode
    {
    public:
        explicit constexpr MatchClauseSyntax(SyntaxKind kind)
            : SyntaxNode{kind}
        {
        }
    };

    class MatchExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(MatchExpressionSyntax);

    public:
        SyntaxToken* MatchKeyword{};
        SyntaxListView<AttributeListSyntax> ConditionAttributes{};
        SyntaxToken* OpenParenToken{};
        ExpressionSyntax* Condition{};
        SyntaxToken* CloseParenToken{};

        UnexpectedNodesSyntax* BeforeLeftBrace{};
        SyntaxToken* LeftBrace{};
        SyntaxListView<MatchClauseSyntax> Elements{};
        UnexpectedNodesSyntax* BeforeRightBrace{};
        SyntaxToken* RightBrace{};

    public:
        explicit constexpr MatchExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::MatchExpressionSyntax}
        {
        }
    };

    class MatchCaseClauseSyntax final : public MatchClauseSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(MatchCaseClauseSyntax);

    public:
        SyntaxToken* CaseKeyword{};
        PatternSyntax* Pattern{};
        SyntaxToken* ColonToken{};
        CodeBlockItemSyntax* Body{};
        SyntaxToken* TrailingSemicolon{};

    public:
        explicit constexpr MatchCaseClauseSyntax()
            : MatchClauseSyntax{SyntaxKind::MatchCaseClauseSyntax}
        {
        }
    };

    class MatchDefaultClauseSyntax final : public MatchClauseSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(MatchDefaultClauseSyntax);

    public:
        SyntaxToken* DefaultKeyword{};
        SyntaxToken* ColonToken{};
        CodeBlockItemSyntax* Body{};
        SyntaxToken* TrailingSemicolon{};

    public:
        explicit constexpr MatchDefaultClauseSyntax()
            : MatchClauseSyntax{SyntaxKind::MatchDefaultClauseSyntax}
        {
        }
    };

    /// pattern
    ///     : wildcard-pattern
    ///     | literal-pattern
    ///     | identifier-pattern
    ///     | slice-pattern
    ///     | tuple-pattern
    ///     | struct-pattern
    class PatternSyntax : public SyntaxNode
    {
    public:
        explicit constexpr PatternSyntax(SyntaxKind kind)
            : SyntaxNode{kind}
        {
        }
    };

    /// wildcard-pattern
    ///     : '_'
    ///     ;
    class WildcardPatternSyntax final : public PatternSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(WildcardPatternSyntax);

    public:
        SyntaxToken* WildcardToken{};

    public:
        explicit constexpr WildcardPatternSyntax()
            : PatternSyntax{SyntaxKind::WildcardPatternSyntax}
        {
        }
    };

    /// literal-pattern
    ///     : literal
    ///     ;
    class LiteralPatternSyntax final : public PatternSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(LiteralPatternSyntax);

    public:
        SyntaxToken* LiteralToken{};

    public:
        explicit constexpr LiteralPatternSyntax()
            : PatternSyntax{SyntaxKind::LiteralPatternSyntax}
        {
        }
    };

    /// identifier-pattern
    ///     : name pattern?
    ///     ;
    class IdentifierPatternSyntax final : public PatternSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(IdentifierPatternSyntax);

    public:
        IdentifierSyntax* Identifier{};
        PatternSyntax* Pattern{};

    public:
        explicit constexpr IdentifierPatternSyntax()
            : PatternSyntax{SyntaxKind::IdentifierPatternSyntax}
        {
        }
    };

    /// slice-pattern-item
    ///     : pattern
    ///     ;
    ///
    /// slice-pattern-item-sequence
    ///     : slice-pattern-item (',' slice-pattern-item)*
    ///     ;
    class SlicePatternItemSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(SlicePatternItemSyntax);

    public:
        PatternSyntax* Pattern{};
        SyntaxToken* TrailingComma{};

    public:
        explicit constexpr SlicePatternItemSyntax()
            : SyntaxNode{SyntaxKind::SlicePatternItemSyntax}
        {
        }
    };

    /// slice-pattern
    ///     : '[' slice-pattern-item-sequence? ']'
    ///     ;
    class SlicePatternSyntax final : public PatternSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(SlicePatternSyntax);

    public:
        SyntaxToken* OpenBracketToken{};
        SyntaxListView<SlicePatternItemSyntax> Items{};
        SyntaxToken* CloseBracketToken{};

    public:
        explicit constexpr SlicePatternSyntax()
            : PatternSyntax{SyntaxKind::SlicePatternSyntax}
        {
        }
    };

    /// tuple-pattern-item
    ///     : name-colon? pattern
    ///     ;
    ///
    /// tuple-pattern-item-sequence
    ///     : tuple-pattern-item (',' tuple-pattern-item)*
    ///     ;
    class TuplePatternItemSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(TuplePatternItemSyntax);

    public:
        NameColonSyntax* Name{};
        PatternSyntax* Pattern{};
        SyntaxToken* TrailingComma{};

    public:
        explicit constexpr TuplePatternItemSyntax()
            : SyntaxNode{SyntaxKind::TuplePatternItemSyntax}
        {
        }
    };

    /// tuple-pattern
    ///     : '(' tuple-pattern-item-sequence? ')'
    ///     ;
    class TuplePatternSyntax final : public PatternSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(TuplePatternSyntax);

    public:
        UnexpectedNodesSyntax* BeforeOpenParenToken{};
        SyntaxToken* OpenParenToken{};
        SyntaxListView<TuplePatternItemSyntax> Items{};
        UnexpectedNodesSyntax* BeforeCloseParenToken{};
        SyntaxToken* CloseParenToken{};

    public:
        explicit constexpr TuplePatternSyntax()
            : PatternSyntax{SyntaxKind::TuplePatternSyntax}
        {
        }
    };

    /// field-pattern
    ///     : identifier ':' pattern
    ///     ;
    ///
    /// field-pattern-sequence
    ///     : field-pattern (',' field-pattern)*
    ///     ;
    class FieldPatternSyntax final : public PatternSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(FieldPatternSyntax);

    public:
        IdentifierSyntax* Name{};
        SyntaxToken* ColonToken{};
        PatternSyntax* Pattern{};
        SyntaxToken* TrailingComma{};

    public:
        explicit constexpr FieldPatternSyntax()
            : PatternSyntax{SyntaxKind::FieldPatternSyntax}
        {
        }
    };

    /// struct-pattern
    ///     : '{' field-pattern-sequence? '}'
    ///     ;
    class StructPatternSyntax final : public PatternSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(StructPatternSyntax);

    public:
        UnexpectedNodesSyntax* BeforeOpenBraceToken{};
        SyntaxToken* OpenBraceToken{};

        SyntaxListView<FieldPatternSyntax> Fields{};

        UnexpectedNodesSyntax* BeforeCloseBraceToken{};
        SyntaxToken* CloseBraceToken{};

    public:
        explicit constexpr StructPatternSyntax()
            : PatternSyntax{SyntaxKind::StructPatternSyntax}
        {
        }
    };

    class TypePatternSyntax final : public PatternSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(TypePatternSyntax);

    public:
        PatternSyntax* Pattern{};
        TypeClauseSyntax* Type{};

    public:
        explicit constexpr TypePatternSyntax()
            : PatternSyntax{SyntaxKind::TypePatternSyntax}
        {
        }
    };

    class PatternOrItemSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(PatternOrItemSyntax);

    public:
        PatternSyntax* Pattern{};
        SyntaxToken* TrailingBarToken{};

    public:
        explicit constexpr PatternOrItemSyntax()
            : SyntaxNode{SyntaxKind::PatternOrItemSyntax}
        {
        }
    };

    /// ```
    /// pattern-core
    ///     :
    ///     ;
    ///
    /// pattern
    ///     : pattern_core ('|' pattern_core)*
    ///     ;
    /// ```
    class PatternOrSyntax final : public PatternSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(PatternOrSyntax);

    public:
        SyntaxListView<PatternOrItemSyntax> Items{};

    public:
        explicit constexpr PatternOrSyntax()
            : PatternSyntax{SyntaxKind::PatternOrSyntax}
        {
        }
    };

    /// type-inheritance-clause
    ///     : ':' type
    ///     ;
    class TypeInheritanceClause final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(TypeInheritanceClause);

    public:
        SyntaxToken* ColonToken{};
        TypeSyntax* BaseType{};

    public:
        explicit constexpr TypeInheritanceClause()
            : SyntaxNode{SyntaxKind::TypeInheritanceClause}
        {
        }
    };

    /// name-colon
    ///     : identifier ':'
    ///     ;
    class NameColonSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(NameColonSyntax);

    public:
        IdentifierSyntax* Name{};
        SyntaxToken* ColonToken{};

    public:
        explicit constexpr NameColonSyntax()
            : SyntaxNode{SyntaxKind::NameColonSyntax}
        {
        }
    };

    /// let-expression-binding-specifier
    ///     : 'let'
    ///     | 'var'
    ///     ;
    ///
    /// let-expression
    ///     : let-expression-binding-specifier pattern-binding
    ///     ;
    class LetExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(LetExpressionSyntax);

    public:
        SyntaxToken* BindingSpecifier{};
        PatternSyntax* Pattern{};
        ExpressionInitializerClauseSyntax* Initializer{};

    public:
        explicit constexpr LetExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::LetExpressionSyntax}
        {
        }
    };

    /// expression-reference-qualifier
    ///     : 'let'
    ///     | 'var'
    ///     ;
    ///
    /// expression-reference
    ///     : '&' expression-reference-qualifier? expression
    ///     ;
    class ExpressionReferenceSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(ExpressionReferenceSyntax);

    public:
        SyntaxToken* AmpersandToken{};
        SyntaxToken* QualifierToken{};
        ExpressionSyntax* Expression{};

    public:
        explicit constexpr ExpressionReferenceSyntax()
            : ExpressionSyntax{SyntaxKind::ExpressionReferenceSyntax}
        {
        }
    };

    /// for-statement
    ///     : 'for' '(' expression ';' expression ';' expression ')' code-block
    ///     ;
    class ForStatementSyntax final : public StatementSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(ForStatementSyntax);

    public:
        SyntaxToken* ForKeyword{};

        UnexpectedNodesSyntax* BeforeOpenParenToken{};
        SyntaxToken* OpenParenToken{};

        ExpressionSyntax* Initializer{};
        SyntaxToken* FirstSemicolonToken{};
        ExpressionSyntax* Condition{};
        SyntaxToken* SecondSemicolonToken{};
        ExpressionSyntax* Expression{};

        UnexpectedNodesSyntax* BeforeCloseParenToken{};
        SyntaxToken* CloseParenToken{};

        CodeBlockSyntax* Body{};

    public:
        explicit constexpr ForStatementSyntax()
            : StatementSyntax{SyntaxKind::ForStatementSyntax}
        {
        }
    };

    /// foreach-statement
    ///     : 'foreach' '(' variable-declaration 'in' expression ')' code-block
    ///     ;
    class ForeachStatementSyntax final : public StatementSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(ForeachStatementSyntax);

    public:
        SyntaxToken* ForeachKeyword{};

        UnexpectedNodesSyntax* BeforeOpenParenToken{};
        SyntaxToken* OpenParenToken{};

        VariableDeclarationSyntax* Variable{};

        SyntaxToken* InKeyword{};

        ExpressionSyntax* Expression{};

        UnexpectedNodesSyntax* BeforeCloseParenToken{};
        SyntaxToken* CloseParenToken{};

        CodeBlockSyntax* Body{};

    public:
        explicit constexpr ForeachStatementSyntax()
            : StatementSyntax{SyntaxKind::ForeachStatementSyntax}
        {
        }
    };


    /// type-path
    ///     : path
    ///     ;
    ///
    /// path
    ///     : path-segment-sequence
    ///     ;
    ///
    /// path-segment-sequence
    ///     : path-segment ('::' path-segment)*
    ///     ;
    ///
    /// path-segment
    ///     : identifier generic-arguments?
    ///     ;
    ///
    /// generic-arguments
    ///     : '![' generic-argument-sequence ']'
    ///     ;
    ///
    /// generic-argument-sequence
    ///     : generic-argument (',' generic-argument)*
    ///     ;
    ///
    /// generic-argument
    ///     : expression
    ///     ;
    ///
    /// path-expression
    ///     : path
    ///     ;

    class NameSyntax : public SyntaxNode
    {
    public:
        explicit constexpr NameSyntax(SyntaxKind kind)
            : SyntaxNode{kind}
        {
        }
    };

    class IdentifierSyntax final : public NameSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(IdentifierSyntax);

    public:
        IdentifierSyntaxToken* Identifier{};

    public:
        explicit constexpr IdentifierSyntax()
            : NameSyntax{SyntaxKind::IdentifierSyntax}
        {
        }
    };

    class IndexSyntax final : public NameSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(IndexSyntax);

    public:
        IntegerLiteralSyntaxToken* Index{};

    public:
        explicit constexpr IndexSyntax()
            : NameSyntax{SyntaxKind::IndexSyntax}
        {
        }
    };

    class PathSegmentSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(PathSegmentSyntax);

    public:
        IdentifierSyntax* Identifier{};
        GenericArgumentsSyntax* Arguments{};
        SyntaxToken* TrailingSeparator{};

    public:
        explicit constexpr PathSegmentSyntax()
            : SyntaxNode{SyntaxKind::PathSegmentSyntax}
        {
        }
    };

    class PathSyntax final : public SyntaxNode
    {
        WEAVE_DEFINE_SYNTAX_NODE(PathSyntax);

    public:
        SyntaxListView<PathSegmentSyntax> Segments{};

    public:
        explicit constexpr PathSyntax()
            : SyntaxNode{SyntaxKind::PathSyntax}
        {
        }
    };

    class TypePathSyntax final : public TypeSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(TypePathSyntax);

    public:
        PathSyntax* Path{};

    public:
        explicit constexpr TypePathSyntax()
            : TypeSyntax{SyntaxKind::TypePathSyntax}
        {
        }
    };

    class PathExpressionSyntax final : public ExpressionSyntax
    {
        WEAVE_DEFINE_SYNTAX_NODE(PathExpressionSyntax);

    public:
        PathSyntax* Path{};

    public:
        explicit constexpr PathExpressionSyntax()
            : ExpressionSyntax{SyntaxKind::PathExpressionSyntax}
        {
        }
    };

}
