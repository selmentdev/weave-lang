code-block-syntax
    : '{' code-block-item-list-syntax? '}'
    ;

code-block-item-list-syntax
    : code-block-item-syntax code-block-item-syntax*
    ;

code-block-item-syntax
    : (declaration-syntax | statement-syntax | expression-syntax) ';'?
    ;

member-block-syntax
    : '{' member-block-item-list-syntax? '}'
    ;

member-block-item-list-syntax
    : member-block-item-syntax member-block-item-syntax*
    ;

member-block-item-syntax
    : declaration-syntax ';'?
    ;

class-declaration-syntax
    : member-block-syntax
    ;

struct-declaration-syntax
    : member-block-syntax
    ;

enum-declaration-syntax
    : member-block-syntax
    ;

concept-declaration-syntax
    : member-block-syntax
    ;

extend-declaration-syntax
    : member-block-syntax
    ;

declaration-modifier-syntax
    : 'public'
    | 'private'
    | 'internal'
    | 'file'
    | 'async'
    | 'sealed'
    | 'required'
    | 'static'
    ;

declaration-modifier-list-syntax
    : declaration-modifier-syntax*
    ;

attribute-specifier-syntax
    : identifier
    | identifier '(' balanced-token-sequence ')'
    ;

attribute-syntax
    : '[<' attribute-specifier-syntax (',' attribute-specifier-syntax)* '>]'
    ;

attribute-list-syntax
    : attribute-syntax*
    ;

declaration-syntax
    : variable-declaration-syntax
    | function-declaration-syntax
    | constant-declaration-syntax
    | concept-declaration-syntax
    | enum-declaration-syntax
    | class-declaration-syntax
    | struct-declaration-syntax
    | extend-declaration-syntax
    | namespace-declaration-syntax
    ;

variable-declaration-syntax
    : attribue-list-syntax? declaration-modifier-list-syntax? pattern-binding
    ;

pattern-binding
    : identifier-pattern type-annotation? initializer-clause?

identifier-pattern
    : identifier
    ;

type-annotation
    : ':' qualified-identifier
    ;

initializer-clause
    : '=' expression
    ;
