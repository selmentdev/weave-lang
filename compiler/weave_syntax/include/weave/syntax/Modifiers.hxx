#pragma once
#include "weave/bitwise/Flag.hxx"

namespace weave::syntax
{
    /// ```
    /// struct_modifier
    ///     : 'public'
    ///     | 'private'
    ///     | 'internal'
    ///     | 'partial'
    ///     ;
    /// ```
    enum class StructModifier : uint8_t
    {
        None = 0u,
        Public = 1u << 0u,
        Private = 1u << 1u,
        Internal = 1u << 2u,
        Partial = 1u << 3u,
    };

    /// ```
    /// extend_modifier
    ///     : 'public'
    ///     | 'private'
    ///     | 'internal'
    ///     | 'unsafe'
    ///     ;
    /// ```
    enum class ExtendModifier : uint8_t
    {
        None = 0u,
        Public = 1u << 0u,
        Private = 1u << 1u,
        Internal = 1u << 2u,
        Unsafe = 1u << 3u,
    };

    /// ```
    /// concept_modifier
    ///     : 'public'
    ///     | 'private'
    ///     | 'internal'
    ///     | 'unsafe'
    ///     ;
    /// ```
    enum class ConceptModifier : uint8_t
    {
        None = 0u,
        Public = 1u << 0u,
        Private = 1u << 1u,
        Internal = 1u << 2u,
        Unsafe = 1u << 3u,
    };

    /// ```
    /// field_modifier
    ///     : 'public'
    ///     | 'private'
    ///     | 'internal'
    ///     | 'readonly'
    ///     ;
    /// ```
    enum class FieldModifier
    {
        Public,
        Private,
        Internal,
        Readonly,
    };

    /// ```
    /// function_modifier
    ///     : 'public'
    ///     | 'private'
    ///     | 'internal'
    ///     | 'unsafe'
    ///     | 'async'
    ///     | 'extern'
    ///     ;
    /// ```
    enum class FunctionModifier : uint8_t
    {
        None = 0u,
        Public = 1u << 0u,
        Private = 1u << 1u,
        Internal = 1u << 2u,
        Unsafe = 1u << 3u,
        Async = 1u << 4u,
        Extern = 1u << 5u,
        Native = 1u << 6u,
    };

    /*
    | Modifier | Direction | Mutability | Semantics | Ownership | Initialization |
    | -------- | --------- | ---------- | --------- | --------- | -------------- |
    | 'ref'    | both      | readwrite  | reference | shared    | by caller      |
    | 'out'    | output    | writeonly  | reference | unique    | by callee      |
    | 'in'     | input     | readonly   | reference | shared    | by callee      |
    | 'copy'   | input     | readwrite  | value     | unique    | by callee      |
    | 'move'   | input     | readwrite  | value     | unique    | by callee      |
    | 'params' | input     | readonly   | array     | shared    | by callee      |
    */

    /// ```
    /// formal_parameter_modifier
    ///     : 'ref'
    ///     | 'out'
    ///     | 'in'
    ///     | 'const'
    ///     ;
    /// ```
    enum class FormalParameterModifier : uint8_t
    {
        None = 0u,
        In = 1u << 0u,          // Immutable reference
        Out = 1u << 1u,         // Output reference
        Ref = 1u << 2u,         // Mutable reference
        Copy = 1u << 3u,
        Move = 1u << 4u,
        Params = 1u << 5u,      // Variadic params packed into implicit slice array of type
    };

}