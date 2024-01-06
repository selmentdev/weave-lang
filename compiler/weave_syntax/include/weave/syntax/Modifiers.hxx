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
        Ref = 1u << 0u,
        Out = 1u << 1u,
        In = 1u << 2u,
        Const = 1u << 3u,
        Move = 1u << 4u,
    };
}
