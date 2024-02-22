# Command line arguments

## Options

```
Miscellaneous:

    -help
    -version
    -verbose
    -nologo

Output files:

    -o:output <path>                    Output path
    -o:immediate <path>                 Output path for immediate files
    -o:generated <path>                 Output path for generated files
    -o:target <kind>                    Specifies kind of the target.
        app                             Application
        console                         Console Application
        library                         Library
        module                          Module
    -o:name <name>                      Specifies the name of the module.
    -o:documentation <path>             Output path for documentation files

Input files:

    -i:reference <file>                 Adds a reference to the specified module.

Code Generation:

    -c:checked                          Enables overflow checks
    -c:debug                            Emits debug information
    -c:unsafe                           Allows unsafe code
    -c:optimize <level>                 Sets the optimization level to <level>
    -c:define <name>                    Defines a configuration option
    -c:platform <platform>              Specifies the target platform
        linux                           Linux
        windows                         Windows
        macos                           macOS
    -c:architecture <architecture>      Specifies the target architecture
    -c:deterministic                    Generates deterministic output
    -c:coverage                         Generates coverage information
    -c:sanitize-address                 Enables AddressSanitizer
    -c:sanitize-thread                  Enables ThreadSanitizer
    -c:sanitize-memory                  Enables MemorySanitizer
    -c:sanitize-undefined               Enables UndefinedBehaviorSanitizer

Errors and warnings:

    -w:level <level>                    Sets the warning level to <level>
        0                               Turns off all warnings
        1                               Shows only severe warnings
        2                               Shows all warnings
    -w:error                            Treats warnings as errors
    -w:suppress <warning>               Suppresses a warning
    -w:enable <warning>                 Enables a warning
    -w:disable <warning>                Disables a warning

Resources:

    -r:resource <file>                  Adds a resource file to the module.

Analysis:

    -a:analyze                          Analyzes the input source

Emit options:

    -e:documentation                    Generates documentation
    -e:dependency                       Generates dependency information
    -e:metadata                         Generates type information
    -e:assembly-header                  Generates header file with assembly definitions

Experimental Options:

    -x:print-syntax-tree <format>       Prints syntax tree of the input source as desired format.
    -x:print-semantic-tree <format>     Prints semantic tree of the input source as desired format.
```

