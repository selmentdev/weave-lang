#pragma once
#include "weave/syntax/SyntaxTree.hxx"
#include "weave/source/SourceText.hxx"

namespace weave::driver
{
    std::string GenerateDocumentation(
        syntax::SourceFileSyntax const& root,
        source::SourceText const& source);
}
