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

        UInt8,
        UInt16,
        UInt32,
        UInt64,
        UInt128,

        Int8,
        Int16,
        Int32,
        Int64,
        Int128,

        Float16,
        Float32,
        Float64,
    };

    struct TokenData final
    {
        enum class ValueKind
        {
            None,
            Integer,
            Float,
            String,
        } Kind;

        union ValueStorage
        {
            struct StringValue
            {
                BuiltinType Type;
                std::string Value;
            } String;
            struct IntegerValue
            {
                BuiltinType Type;
                Builtin::UInt128 Value;
            } Integer;
            struct FloatValue
            {
                BuiltinType Type;
                double Value;
            } Float;

            ValueStorage() { }
            ~ValueStorage() { }
        } Value;

        TokenData(TokenData const& other)
            : Kind{other.Kind}
        {
            switch (other.Kind)
            {
            default:
                {
                    WEAVE_ASSERT(false);
                    break;
                }

            case ValueKind::None:
                {
                    break;
                }

            case ValueKind::String:
                {
                    std::construct_at(&this->Value.String, other.Value.String);
                    break;
                }

            case ValueKind::Float:
                {
                    std::construct_at(&this->Value.Float, other.Value.Float);
                    break;
                }

            case ValueKind::Integer:
                {
                    std::construct_at(&this->Value.Integer, other.Value.Integer);
                    break;
                }
            }
        }

        TokenData(TokenData&& other)
            : Kind{other.Kind}
        {
            switch (other.Kind)
            {
            default:
                {
                    WEAVE_ASSERT(false);
                    break;
                }

            case ValueKind::None:
                {
                    break;
                }
            }
        }
    };
}
