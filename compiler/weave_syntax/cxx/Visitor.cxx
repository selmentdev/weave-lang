#include "weave/syntax/Visitor.hxx"

namespace weave::syntax
{
    void SyntaxWalker::OnCompilationUnitSyntax(CompilationUnitSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->AttributeLists.GetNode());
        this->Dispatch(node->Usings.GetNode());
        this->Dispatch(node->Members.GetNode());
        this->Dispatch(node->EndOfFileToken);

        --this->Depth;
    }

    void SyntaxWalker::OnSyntaxList(SyntaxList const* node)
    {
        ++this->Depth;

        size_t const count = node->GetCount();
        SyntaxNode const** elements = node->GetElements();

        for (size_t i = 0; i < count; ++i)
        {
            this->Dispatch(elements[i]);
        }

        --this->Depth;
    }

    void SyntaxWalker::OnNamespaceDeclarationSyntax(NamespaceDeclarationSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->NamespaceKeyword);
        this->Dispatch(node->Name);
        this->Dispatch(node->OpenBraceToken);
        this->Dispatch(node->Usings.GetNode());
        this->Dispatch(node->Members.GetNode());
        this->Dispatch(node->CloseBraceToken);
        this->Dispatch(node->SemicolonToken);

        --this->Depth;
    }

    void SyntaxWalker::OnStructDeclarationSyntax(StructDeclarationSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->StructKeyword);
        this->Dispatch(node->Name);
        this->Dispatch(node->OpenBraceToken);
        this->Dispatch(node->Members.GetNode());
        this->Dispatch(node->CloseBraceToken);
        this->Dispatch(node->SemicolonToken);

        --this->Depth;
    }

    void SyntaxWalker::OnConceptDeclarationSyntax(ConceptDeclarationSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->ConceptKeyword);
        this->Dispatch(node->Name);
        this->Dispatch(node->OpenBraceToken);
        this->Dispatch(node->Members.GetNode());
        this->Dispatch(node->CloseBraceToken);
        this->Dispatch(node->SemicolonToken);

        --this->Depth;
    }

    void SyntaxWalker::OnExtendDeclarationSyntax(ExtendDeclarationSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->ExtendKeyword);
        this->Dispatch(node->Name);
        this->Dispatch(node->OpenBraceToken);
        this->Dispatch(node->Members.GetNode());
        this->Dispatch(node->CloseBraceToken);
        this->Dispatch(node->SemicolonToken);

        --this->Depth;
    }

    void SyntaxWalker::OnIncompleteDeclarationSyntax(IncompleteDeclarationSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->Type);

        --this->Depth;
    }
    void SyntaxWalker::OnQualifiedNameSyntax(QualifiedNameSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->Left);
        this->Dispatch(node->DotToken);
        this->Dispatch(node->Right);

        --this->Depth;
    }

    void SyntaxWalker::OnFunctionDeclarationSyntax(FunctionDeclarationSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->FunctionKeyword);
        this->Dispatch(node->Name);
        this->Dispatch(node->Parameters);
        this->Dispatch(node->ArrowToken);
        this->Dispatch(node->ReturnType);
        this->Dispatch(node->OpenBraceToken);
        this->Dispatch(node->CloseBraceToken);

        --this->Depth;
    }
    void SyntaxWalker::OnUsingDirectiveSyntax(UsingDirectiveSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->UsingKeyword);
        this->Dispatch(node->Name);
        this->Dispatch(node->SemicolonToken);

        --this->Depth;
    }
    void SyntaxWalker::OnIdentifierNameSyntax(IdentifierNameSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->Identifier);

        --this->Depth;
    }
    void SyntaxWalker::OnParameterListSyntax(ParameterListSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->OpenParenToken);
        this->Dispatch(node->Parameters.GetNode());
        this->Dispatch(node->CloseParenToken);

        --this->Depth;
    }
    void SyntaxWalker::OnParameterSyntax(ParameterSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->Attributes.GetNode());
        this->Dispatch(node->Modifiers.GetNode());
        this->Dispatch(node->Identifier);
        this->Dispatch(node->Type);

        --this->Depth;
    }

    void SyntaxWalker::OnTypeClauseSyntax(TypeClauseSyntax const* node)
    {
        ++this->Depth;

        this->Dispatch(node->ColonToken);
        this->Dispatch(node->Identifier);

        --this->Depth;
    }
}
