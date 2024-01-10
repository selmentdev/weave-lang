#include "weave/syntax/Visitor.hxx"

namespace weave::syntax
{
    SyntaxWalker::~SyntaxWalker()
    {
    }

    void SyntaxWalker::VisitCompilationUnitDeclaration(CompilationUnitDeclaration* node)
    {
        ++this->Depth;

        for (auto m : node->Usings)
        {
            this->Visit(m);
        }

        for (auto m : node->Members)
        {
            this->Visit(m);
        }

        --this->Depth;
    }

    void SyntaxWalker::VisitNamespaceDeclaration(NamespaceDeclaration* node)
    {
        ++this->Depth;

        this->Visit(node->Name);

        for (auto m : node->Usings)
        {
            this->Visit(m);
        }

        for (auto n : node->Members)
        {
            this->Visit(n);
        }

        --this->Depth;
    }

    void SyntaxWalker::VisitUsingStatement(UsingStatement* node)
    {
        ++this->Depth;

        this->Visit(node->Name);

        --this->Depth;
    }

    void SyntaxWalker::VisitQualifiedNameExpression(QualifiedNameExpression* node)
    {
        ++this->Depth;

        this->Visit(node->Left);
        this->Visit(node->Right);

        --this->Depth;
    }

    void SyntaxWalker::VisitStructDeclaration(StructDeclaration* node)
    {
        ++this->Depth;

        this->Visit(node->Name);

        for (auto m : node->Members)
        {
            this->Visit(m);
        }

        --this->Depth;
    }


    void SyntaxWalker::VisitConceptDeclaration(ConceptDeclaration* node)
    {
        ++this->Depth;

        this->Visit(node->Name);

        for (auto m : node->Members)
        {
            this->Visit(m);
        }

        --this->Depth;
    }

    void SyntaxWalker::VisitExtendDeclaration(ExtendDeclaration* node)
    {
        ++this->Depth;

        this->Visit(node->Name);

        for (auto m : node->Members)
        {
            this->Visit(m);
        }

        --this->Depth;
    }
    void SyntaxWalker::VisitFunctionDeclaration(FunctionDeclaration* node)
    {
        ++this->Depth;

        this->Visit(node->Name);

        if (node->ReturnType)
        {
            this->Visit(node->ReturnType);
        }

        --this->Depth;
    }

    void SyntaxWalker::VisitFieldDeclaration(FieldDeclaration* node)
    {
        ++this->Depth;

        this->Visit(node->Name);

        if (node->Type != nullptr)
        {
            this->Visit(node->Type);
        }

        if (node->Initializer != nullptr)
        {
            this->Visit(node->Initializer);
        }

        --this->Depth;
    }

    void SyntaxWalker::VisitConstantDeclaration(ConstantDeclaration* node)
    {
        ++this->Depth;

        this->Visit(node->Name);

        if (node->Type != nullptr)
        {
            this->Visit(node->Type);
        }

        if (node->Initializer != nullptr)
        {
            this->Visit(node->Initializer);
        }

        --this->Depth;
    }
}


#include "weave/syntax2/Visitor.hxx"

namespace weave::syntax2
{
}
