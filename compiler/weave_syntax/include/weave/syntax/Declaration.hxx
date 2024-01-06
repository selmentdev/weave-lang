#pragma once
#include "weave/syntax/SyntaxNode.hxx"
#include "weave/syntax/Modifiers.hxx"

namespace weave::syntax
{
    class Declaration : public SyntaxNode
    {
    public:
        explicit constexpr Declaration(SyntaxKind kind)
            : SyntaxNode{kind}
        {
        }
    };

    class CompilationUnitDeclaration final : public Declaration
    {
    public:
        static constexpr bool ClassOf(SyntaxKind kind)
        {
            return kind == SyntaxKind::CompilationUnitDeclaration;
        }

        static constexpr bool ClassOf(Declaration const* n)
        {
            return ClassOf(n->Kind());
        }

    public:
        explicit constexpr CompilationUnitDeclaration()
            : Declaration{SyntaxKind::CompilationUnitDeclaration}
        {
        }

    public:
        std::span<MemberDeclaration*> Members{};
        std::span<UsingStatement*> Usings{};
        tokenizer::Token* EndOfFile{};
    };

    class MemberDeclaration : public Declaration
    {
    public:
        explicit constexpr MemberDeclaration(SyntaxKind kind)
            : Declaration{kind}
        {
        }
    };

    class IncompleteMemberDeclaration : public MemberDeclaration
    {
    public:
        static constexpr bool ClassOf(SyntaxKind kind)
        {
            return kind == SyntaxKind::IncompleteMemberDeclaration;
        }

        static constexpr bool ClassOf(MemberDeclaration const* n)
        {
            return ClassOf(n->Kind());
        }

    public:
        explicit constexpr IncompleteMemberDeclaration(std::span<tokenizer::Token*> tokens)
            : MemberDeclaration{SyntaxKind::IncompleteMemberDeclaration}
            , Tokens{tokens}
        {
        }

    public:
        std::span<tokenizer::Token*> Tokens{};
    };

    /// ```
    /// member_declaration
    ///     : incomplete_member_declaration
    ///     | namespace_declaration
    ///     | struct_declaration
    ///     | extend_declaration
    ///     | constant_declaration
    ///     ;
    ///
    /// namespace_declaration
    ///     : 'namespace' qualified_name '{' using_statement* member_declaration* '}'
    ///     | 'namespace' qualified_name ';' using_statement* member_declaration*
    ///     ;
    /// ```
    class NamespaceDeclaration final : public MemberDeclaration
    {
    public:
        static constexpr bool ClassOf(SyntaxKind kind)
        {
            return kind == SyntaxKind::NamespaceDeclaration;
        }

        static constexpr bool ClassOf(MemberDeclaration const* n)
        {
            return ClassOf(n->Kind());
        }

    public:
        explicit constexpr NamespaceDeclaration()
            : MemberDeclaration{SyntaxKind::NamespaceDeclaration}
        {
        }

    public:
        tokenizer::Token* NamespaceKeyword{};
        NameExpression* Name{};
        tokenizer::Token* OpenBraceToken{};
        std::span<UsingStatement*> Usings{};
        std::span<MemberDeclaration*> Members{};
        tokenizer::Token* CloseBraceToken{};
    };

    class StructDeclaration : public MemberDeclaration
    {
    public:
        static constexpr bool ClassOf(SyntaxKind kind)
        {
            return kind == SyntaxKind::StructDeclaration;
        }

        static constexpr bool ClassOf(MemberDeclaration const* n)
        {
            return ClassOf(n->Kind());
        }

    public:
        explicit constexpr StructDeclaration()
            : MemberDeclaration{SyntaxKind::StructDeclaration}
        {
        }

    public:
        bitwise::Flags<StructModifier> Modifiers{};
        tokenizer::Token* StructKeyword{};
        NameExpression* Name{};
        tokenizer::Token* OpenBraceToken{};
        std::span<MemberDeclaration*> Members{};
        tokenizer::Token* CloseBraceToken{};
        tokenizer::Token* SemicolonToken{};
    };

    class ExtendDeclaration : public MemberDeclaration
    {
    public:
        static constexpr bool ClassOf(SyntaxKind kind)
        {
            return kind == SyntaxKind::ExtendDeclaration;
        }

        static constexpr bool ClassOf(MemberDeclaration const* n)
        {
            return ClassOf(n->Kind());
        }

    public:
        tokenizer::Token* ExtendKeyword{};
        NameExpression* Name{};
        tokenizer::Token* OpenBraceToken{};
        std::span<MemberDeclaration*> Members{};
        tokenizer::Token* CloseBraceToken{};
    };

    /// ```
    /// field_name
    ///     : identifier
    ///     ;
    /// field_declaration
    ///     : field_modifier* 'var' field_name ':' type ';'
    ///     ;
    /// ```
    class FieldDeclaration : public MemberDeclaration
    {
    public:
        static constexpr bool ClassOf(SyntaxKind kind)
        {
            return kind == SyntaxKind::FieldDeclaration;
        }

        static constexpr bool ClassOf(MemberDeclaration const* n)
        {
            return ClassOf(n->Kind());
        }

    public:
        bitwise::Flags<FieldModifier> Modifiers{};
    };

    /// ```
    /// self_parameter
    ///     : formal_parameter_modifier* 'self'
    ///     ;
    ///
    /// formal_parameter_list
    ///     : self_parameter? (',' formal_parameter)* (',' variadic_parameter)?
    ///     ;
    /// ```
    ///
    /// ```
    /// generic_parameter_type
    ///     : 'type' type_name ('=' type_name)?
    ///
    /// generic_parameter_const
    ///     : 'const' identifier ':' type_name ('=' expression)?
    ///
    /// generic_parameter
    ///     : generic_parameter_type
    ///     | generic_parameter_const
    ///     ;
    ///
    /// generic_parameters
    ///     : generic_parameter (',' generic_parameter)*
    ///     ;
    ///
    /// generic_parameter_list
    ///     : '![' generic_parameters ']'
    ///     ;
    /// ```

    /// ```
    /// function_constraint_type
    ///     : 'where' identifier ':' type ('+' type)*
    ///     ;
    /// function_constraint_expression
    ///     : 'where' boolean_expression
    ///     ;
    /// function_constraint
    ///     : function_constraint_type | function_constraint_expression
    ///     ;
    ///
    /// function_constraints_list
    ///     : function_constraint (',' function_constraint)*
    ///
    /// function_name
    ///     : identifier
    ///     ;
    ///
    /// function_return_type
    ///     : '->' type
    ///     ;
    ///
    /// function_declaration
    ///     : function_modifier* 'function' function_name generic_parameter_list? '(' formal_parameter_list? ')' function_return_type function_constraints_list? function_body
    ///     ;
    /// ```
    class FunctionDeclaration : public MemberDeclaration
    {
    public:
        static constexpr bool ClassOf(SyntaxKind kind)
        {
            return kind == SyntaxKind::FunctionDeclaration;
        }

        static constexpr bool ClassOf(MemberDeclaration const* n)
        {
            return ClassOf(n->Kind());
        }

    public:
        tokenizer::Token* FunctionKeyword{};
        bitwise::Flags<FunctionModifier> Modifiers{};
    };

    class ConstantDeclaration : public MemberDeclaration
    {
    };
}
