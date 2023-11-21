# Command line arguments

## Options

`-c, --codegen` `<name>=<value>` - code generator options
`-e <options>` - emit options
`-o output` - specifies output directory; `weavec` emits additional compilation artifacts here, based on `-e` parameters
`-t <name>=<value>` - specifies target options
`-x <name>=<value>` - experimental options, usually used for debugging `weavec`
`-k <name>=<value>` - linter options

### Code Generator options

| Option | Description |
|-|-|
| `-c checked` | Enables overflow checks |
| `-c define=NAME` | Defines a preprocessor symbol of `NAME` |
| `-c opt=<level>` | Sets the optimization level to `level` |
| `-c debug` | Emits debug information |
| `-c platform=<platform>` | Specifies the platform |
| `-c target=<target>` | Specifies the target. May be app, library, module |
| `-c name=<name>` | Specifies the name of the module |

### Emit options

| Option | Description | Notes |
|-|-|-|
| `-e doc` | Generates documentation | |
| `-e dep` | Generates dependency information | |
| `-e meta` | Generates type information | Default, needed to link metadata. |
| `-e asm-header` | Generates assembly header for types and constants | This tool may be experimental. Only some files would require that option (like native assembly sources) |

### Experimental

| Option | Description |
|-|-|
| `-x print=<format>` | Prints input source as desired format. This may be done at various stages of compilation. |
