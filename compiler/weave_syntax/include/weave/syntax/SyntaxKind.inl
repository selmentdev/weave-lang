#ifndef WEAVE_SYNTAX_NODE
#define WEAVE_SYNTAX_NODE(name, spelling)
#endif

#ifndef WEAVE_SYNTAX_ABSTRACT_NODE
#define WEAVE_SYNTAX_ABSTRACT_NODE(name, spelling) WEAVE_SYNTAX_NODE(name, spelling)
#endif

#ifndef WEAVE_SYNTAX_CONCRETE_NODE
#define WEAVE_SYNTAX_CONCRETE_NODE(name, spelling) WEAVE_SYNTAX_NODE(name, spelling)
#endif

#ifndef WEAVE_SYNTAX_DECLARATION
#define WEAVE_SYNTAX_DECLARATION(name, spelling) WEAVE_SYNTAX_CONCRETE_NODE(name, spelling)
#endif

#ifndef WEAVE_SYNTAX_EXPRESSION
#define WEAVE_SYNTAX_EXPRESSION(name, spelling) WEAVE_SYNTAX_CONCRETE_NODE(name, spelling)
#endif

#ifndef WEAVE_SYNTAX_STATEMENT
#define WEAVE_SYNTAX_STATEMENT(name, spelling) WEAVE_SYNTAX_CONCRETE_NODE(name, spelling)
#endif


// clang-format off

WEAVE_SYNTAX_ABSTRACT_NODE(Declaration,                         "[syntax::declaration]")
WEAVE_SYNTAX_ABSTRACT_NODE(Expression,                          "[syntax::expression]")
WEAVE_SYNTAX_ABSTRACT_NODE(Statement,                           "[syntax::statement]")

WEAVE_SYNTAX_DECLARATION(CompilationUnitDeclaration,            "[declaration::compilation-unit]")
WEAVE_SYNTAX_DECLARATION(MemberDeclaration,                     "[declaration::member]")
WEAVE_SYNTAX_DECLARATION(StructDeclaration,                     "[declaration::struct]")
WEAVE_SYNTAX_DECLARATION(ExtendDeclaration,                     "[declaration::extend]")
WEAVE_SYNTAX_DECLARATION(ConceptDeclaration,                    "[declaration::concept]")
WEAVE_SYNTAX_DECLARATION(FunctionDeclaration,                   "[declaration::function]")
WEAVE_SYNTAX_DECLARATION(FieldDeclaration,                      "[declaration::field]")
WEAVE_SYNTAX_DECLARATION(IncompleteMemberDeclaration,           "[declaration::incomplete-member]")
WEAVE_SYNTAX_DECLARATION(NamespaceDeclaration,                  "[declaration::namespace]")
WEAVE_SYNTAX_DECLARATION(GenericParameterTypeDeclaration,       "[declaration::generic-parameter-type]")
WEAVE_SYNTAX_DECLARATION(GenericParameterConstDeclaration,      "[declaration::generic-parameter-const]")

WEAVE_SYNTAX_EXPRESSION(NameExpression,                         "[expression::name]")
WEAVE_SYNTAX_EXPRESSION(QualifiedNameExpression,                "[expression::qualified-name]")
WEAVE_SYNTAX_EXPRESSION(SimpleNameExpression,                   "[expression::simple-name]")
WEAVE_SYNTAX_EXPRESSION(IdentifierNameExpression,               "[expression::identifier-name]")
WEAVE_SYNTAX_EXPRESSION(GenericNameExpression,                  "[expression::generic-name]")

WEAVE_SYNTAX_STATEMENT(EmptyStatement,                          "[statement::empty]")
WEAVE_SYNTAX_STATEMENT(BlockStatement,                          "[statement::block]")
WEAVE_SYNTAX_STATEMENT(UsingStatement,                          "[statement::using]")
WEAVE_SYNTAX_STATEMENT(IfStatement,                             "[statement::if]")
WEAVE_SYNTAX_STATEMENT(ElseStatement,                           "[statement::else]")

// clang-format on

#undef WEAVE_SYNTAX_NODE
#undef WEAVE_SYNTAX_ABSTRACT_NODE
#undef WEAVE_SYNTAX_CONCRETE_NODE
#undef WEAVE_SYNTAX_DECLARATION
#undef WEAVE_SYNTAX_EXPRESSION
#undef WEAVE_SYNTAX_STATEMENT
