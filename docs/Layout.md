# Directories layout

| Directory                          | Description                                                                   | Module Name      | Project Name            |
|------------------------------------|-------------------------------------------------------------------------------|------------------|-------------------------|
| /out                               |                                                                               |                  |                         |
| /docs                              |                                                                               |                  |                         |
| /scripts                           |                                                                               |                  |                         |
| /src/libraries/Core                | core and standard library                                                     |                  | `std`                   |
| /src/native/Core.Runtime           | native runtime libary; supplementary native implementation of intrinsics etc. |                  | `core.runtime`          |
| /src/native/Core.Sanitize          | sanitizers library                                                            |                  | `core.sanitize`         |
| /src/native/Core.Profiler          | profiler library                                                              |                  | `core.profiler`         |
| /src/compiler                      | compiler implementation                                                       |                  |                         |
| /src/compiler/Build                | build system                                                                  | `weave-build`    | `weave.build`           |
| /src/compiler/Package              | packaging tool                                                                | `weave-package`  | `weave.package`         |
| /src/compiler/Frontend             | frontend, metadata dumping tool, compiler executable                          | `weave-frontend` | `weave.frontend`        |
| /src/compiler/LanguageServer       | language server                                                               |                  | `weave.language_server` |
| /src/compiler/Lexer                | lexer                                                                         |                  | `weave.lexer`           |
| /src/compiler/CodeGen              | code generation API                                                           |                  | `weave.codegen`         |
| /src/compiler/CodeGen/modules/C    | code generation for C                                                         |                  | `weave.codegen.c`       |
| /src/compiler/CodeGen/modules/LLVM | code generation for LLVM                                                      |                  | `weave.codegen.llvm`    |
| /src/compiler/Syntax               | syntax ast                                                                    |                  | `weave.syntax`          |
| /src/compiler/ConstEval            | constant evaluation of executable program                                     |                  | `weave.consteval`       |
| /src/compiler/Formatter            | source code formatter                                                         |                  | `weave.formatter`       |
| /src/compiler/Intermediate         | intermediate program representation                                           |                  | `weave.intermediate`    |
| /src/compiler/Lowering             | lowering                                                                      |                  | `weave.lowering`        |
| /src/compiler/Optimization         | optimizations                                                                 |                  | `weave.optimization`    |
| /src/compiler/Passes               | passes framework                                                              |                  | `weave.passes`          |
| /src/compiler/TypeChecker          | type checker                                                                  |                  | `weave.typechecker`     |
| /src/compiler/Tracing              | tracing                                                                       |                  | `weave.tracing`         |
| /src/compiler/Metadata             | metadata                                                                      |                  | `weave.metadata`        |
| /src/compiler/Analyzers            | analyzers                                                                     |                  | `weave.analyzers`       |
| /src/compiler/CodeStyle            | linter?                                                                       |                  | `weave.codestyle`       |
| /src/compiler/Scripting            | Application for executing scripts                                             |                  | `weave.scripting`       |
| /src/tests                         | various tests alogn with runners                                              |                  |                         |
| /src/tools                         | additional tools                                                              |                  |                         |
| /src/samples                       | language samples                                                              |                  |                         |
| /src/thirdparty                    | third party libraries                                                         |                  |                         |

## Project module layout

```
|-- ${module}
    |-- docs                -- documentation
    |-- src                 -- Native source code of module
    |-- include             -- public C++ headers
    |-- cxx                 -- C++ source code of module
    |-- gen                 -- source code generators
    |-- tests               -- module tests
    |-- examples            -- examples of module usage
    |-- benchmarks          -- benchmarks
    |-- ref                 -- reference implementation?
    |-- modules             -- submodules
        |-- ${module}
    |-- tools               -- additional tool modules
        |-- ${module}
```
