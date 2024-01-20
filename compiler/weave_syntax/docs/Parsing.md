# Parsing

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
