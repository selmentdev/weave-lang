#ifndef WEAVE_SYNTAX_NODE
#define WEAVE_SYNTAX_NODE(name, spelling)
#endif

#ifndef WEAVE_SYNTAX_ABSTRACT_NODE
#define WEAVE_SYNTAX_ABSTRACT_NODE(name, spelling) WEAVE_SYNTAX_NODE(name, spelling)
#endif

#ifndef WEAVE_SYNTAX_DECLARATION
#define WEAVE_SYNTAX_DECLARATION(name, spelling) WEAVE_SYNTAX_NODE(name, spelling)
#endif

#ifndef WEAVE_SYNTAX_EXPRESSION
#define WEAVE_SYNTAX_EXPRESSION(name, spelling) WEAVE_SYNTAX_NODE(name, spelling)
#endif

#ifndef WEAVE_SYNTAX_STATEMENT
#define WEAVE_SYNTAX_STATEMENT(name, spelling) WEAVE_SYNTAX_NODE(name, spelling)
#endif


// clang-format off

WEAVE_SYNTAX_NODE(CompilationUnit,                      "[syntax::compilation-unit]")

WEAVE_SYNTAX_ABSTRACT_NODE(DeclarationSyntax,           "[syntax::declaration]")
WEAVE_SYNTAX_ABSTRACT_NODE(ExpressionSyntax,            "[syntax::expression]")
WEAVE_SYNTAX_ABSTRACT_NODE(StatementSyntax,             "[syntax::statement]")

WEAVE_SYNTAX_DECLARATION(MemberDeclaration,             "[declaration::member]")
WEAVE_SYNTAX_DECLARATION(IncompleteMemberDeclaration,   "[declaration::incomplete-member]")
WEAVE_SYNTAX_DECLARATION(StructDeclaration,             "[declaration::struct]")
WEAVE_SYNTAX_DECLARATION(NamespaceDeclaration,          "[declaration::namespace]")

WEAVE_SYNTAX_EXPRESSION(NameExpression,                 "[expression::name]")
WEAVE_SYNTAX_EXPRESSION(QualifiedNameExpression,        "[expression::qualified-name]")
WEAVE_SYNTAX_EXPRESSION(SimpleNameExpression,           "[expression::simple-name]")
WEAVE_SYNTAX_EXPRESSION(IdentifierNameExpression,       "[expression::identifier-name]")
WEAVE_SYNTAX_EXPRESSION(GenericNameExpression,          "[expression::generic-name]")

WEAVE_SYNTAX_STATEMENT(EmptyStatement,                  "[statement::empty]")
WEAVE_SYNTAX_STATEMENT(BlockStatement,                  "[statement::block]")
WEAVE_SYNTAX_STATEMENT(UsingStatement,                  "[statement::using]")
WEAVE_SYNTAX_STATEMENT(IfStatement,                     "[statement::if]")
WEAVE_SYNTAX_STATEMENT(ElseStatement,                   "[statement::else]")

// clang-format on

#undef WEAVE_SYNTAX_NODE
#undef WEAVE_SYNTAX_ABSTRACT_NODE
#undef WEAVE_SYNTAX_DECLARATION
#undef WEAVE_SYNTAX_EXPRESSION
#undef WEAVE_SYNTAX_STATEMENT
