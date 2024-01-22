# Parsing

## Error production

Syntax tree parser won't produce any errors.

Parser can handle unknown token in two ways:

- create missing token
- match until expected token and create list of unexpected tokens

Grammar is defined at semantic level and allows to unambigously parse any provided source code.

Every syntax element has unique introducer. Parser completely ignores context of currently parsed element.

From syntax point of view, there are no differences between:

- global, local and member variables
- global, local and member functions
- expressions can be used at global and function scope

This way we even can introduce new namespace at function level. This does not match actual language grammar, but simplifies parser.

Syntax tree validation is performed after tree is constructed.


## Statement List

- any tokens before `{` are considered unexpected and are appended to list of unexpected nodes
- if we reached end of file, stop parsing
- if we reached `}`, stop parsing
- if we reached `{`, start parsing statements
- when statement was processed successfully, append it to block and continue
- otherwise, treat everything after this point as unexpected nodes, until `}` is reached

### Example

```
if while true
{
    call1();
    @(*#%&*@#);
    call2();
}
```

should be parsed as:

> - syntax(if)
>    - condition(missing)
>    - block statement()
>        - unexpected-nodes()
>            - while
>            - true
>        - open brace
>        - statement-list()
>            - expression-statement()
>              - invoke-expression("call1")
>            - unexpected-nodes()
>        - close brace

this could be modelled as following structure:

```
struct BlockStatement
{
    UnexpectedNodesSyntax* UnexpectedBeforeLeftBrace;
    Token* LeftBrace;
    UnexpectedNodesSyntax* UnexpectedBetweenLeftBraceAndStatements;
    StatementList* Statements;
    UnexpectedNodesSyntax* UnexpectedBetweenStatementsAndRightBrace;
    Token* RightBrace;
    UnexpectedNodesSyntax* UnexpectedAfterRightBrace;
}
```

this way we can consume any unexpected nodes.


## Example 2

Given source code

```
using X;

namespace A {
    extend B {
        function C() {
            var x: float32 = 21.37f32;

// These tokens are not expected
// V___V
}}}}}}}}
// ^---^

            x += 420.69f32;
        }

        var Y : float { return 42; }
    }
}
```

A proper syntax tree will parse to

```
+ Using X;
  + Namespace A;
    + Extend B;
      + Function C; // here matches prefix '}}}' from '}}}}}}}}'
    + Variable Y
      + Unexpected nodes:
        + '}}}}}'
        + x += 420.69f32
        + '}'
      + Declaration
        + 'var Y : ...'
    + Unexpected nodes:
      + '}}'
```
