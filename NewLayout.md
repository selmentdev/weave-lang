# New projects layout

## Introduction

## Current structure

```
|- compiler
    |- weave_frontend
    |- weave_driver
    |- weave_errors             -- error handling
    |- weave_bugcheck           -- bugcheck implementation
    |- weave_syntax
    |- weave_session
    |- weave_system
    |- weave_hash               -- implementation of hash algorithms (both cryptographic and non-cryptographic)
    |- weave_os                 -- implementation of OS-specific functionality (process, pipe etc.)
    |- weave_filesystem         -- implementation of filesystem-related functionality
    |- weave_source             -- source code parsing and processing
    |- weave_json               -- JSON parsing and processing
    |- weave_unicode            -- unicode handling
    |- weave_diagnostic         -- diagnostic messages
    |- weave_profiler           -- profiler
    |- weave_builtin
    |- weave_commandline        -- command line parsing
    |- weave_builtin            -- implementation of builtins
    |- weave_memory             -- memory management and various allocators
|- libraries
    |- core                     -- weave core library (implemented in weave language, transpiled to C/C++ native code)
    |- builtin                  -- C/C++ native implementation of builtins
|- tests
|- third_party
|- tools
```
