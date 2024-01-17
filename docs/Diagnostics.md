# Diagnostics

## Some compiler errors may be over-reported by the parser:

Consider this code

```
function EndOfFileDuringStatement() { a=42:}
```

It causes parser to report this single token more than necessary:

```
error: unexpected token ':', expected ';'
        --> <source>:64:43
         |
       1 | function EndOfFileDuringStatement() { a=42:}
         |                                           ^
error: identifier expected but got :
        --> <source>:64:43
         |
       1 | function EndOfFileDuringStatement() { a=42:}
         |                                           ^
error: unexpected token ':', expected ';'
        --> <source>:64:43
         |
       1 | function EndOfFileDuringStatement() { a=42:}
         |                                           ^
error: unexpected token: ':'
        --> <source>:64:43
         |
       1 | function EndOfFileDuringStatement() { a=42:}
         |
```

Produced syntax tree is:

```
OnCompilationUnitSyntax
    OnSyntaxList
            OnToken `function` `function`
            OnIdentifierNameSyntax `EndOfFileDuringStatement`
                OnToken `(` `[`
                OnToken `)` `)`
                OnToken `{` `{`
                OnSyntaxList
                            OnIdentifierNameSyntax `a`
                            OnToken `=` `=`
                                OnToken `42` `[token:integer-literal]`
                        OnToken `` `;`
                        OnIdentifierNameSyntax ``
                        OnToken `` `;`
                OnToken `}` `}`
    OnToken `` `[token::end-of-file]`
```

Parser tries to recover from the error by inserting a semicolon.

It tries to parse the code as a statement but it fails because of:

- statement cannot start with a ':' so it tries to match identifier
- it inserts artificial identifier before ':' and continues parsing
- it fails like previously - statement cannot end with a ':', so it inserts artificial semicolon
- finally it fails because no statement was produced in block statement - ':' is ignored and removed from syntax tree

This is wrong, because it produces empty statement with artificial identifier and semicolon. This case should just skip ':' and continue parsing.
