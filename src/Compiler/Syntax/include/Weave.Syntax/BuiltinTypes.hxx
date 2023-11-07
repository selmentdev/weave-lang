#pragma once
#include "Weave.Core/Assert.hxx"
#include "Weave.Core/UInt128.hxx"

#include <variant>

namespace Weave
{
    enum class BuiltinType
    {
        Bool,

        Char,
        Char8,
        Char16,
        Char32,

        String,
        String8,
        String16,
        String32,

        USize,
        UIntPtr,

        UInt8,
        UInt16,
        UInt32,
        UInt64,
        UInt128,

        ISize,
        IntPtr,

        Int8,
        Int16,
        Int32,
        Int64,
        Int128,

        Float16,
        Float32,
        Float64,
    };
}
