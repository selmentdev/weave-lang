lang spec

# Weave Language Specification

## List of keywords

- scoping `namespace`, `native`, `extern`
- visibility `public`, `private`, `internal`
- entities `type`, `concept`, `struct`, `enum`, `function`, `extend`, `partial`
- constraints `where`, `assume`, `assert`
- contracts `in`, `out`
- statements `let`, `var`, `if`, `else`, `for`, `while`, `break`, `continue`, `return`, `yield`, `defer`, `match`, `case`, `default`, `using`, `goto`, `loop`, `do`, `const`

## Entities

Each entity can have visibility modifier. There are three visibility modifiers:

- `public` - entity is publicly visible
- `private` - entity is visible only to other functions associated with given type
- `internal` - entity is visible only to other functions defined in same module

## Qualifiers

- none - object is passed by value
- `ptr` - a pointer to object is passed
- `ref` - object is passed by mutable reference - it can be modified
- `in` - object is passed by immutable reference - it cannot be modified
- `out` - function must initialize reference to `out` object before returning
  - `out` keyword must be used when function is called
- `move` - object is moved to function and cannot be used after function call
  - `move` keyword must be used when function is called

Compiler can choose how to pass object using `in` qualifier. Small, trivially copyable objects can be passed by value.

## Functions

Functions are basic building blocks of Weave language.

It is defined by `function` keyword, list of arguments, return type and body.

Example below shows free function defined in global namespace.

```
public function Add(lhs: int32, rhs: int32) -> int32
{
    return lhs + rhs;
}
```

Generic function is a function that has a list of generic parameters. Generic parameter can be either type or compile-time constant.

```
public function Add![type T](lhs: T, rhs: T) -> T
    where T: Addable
{

    return lhs + rhs;
}
```

### Function params

Function parameters are defined by list of arguments. Each argument is defined by name, type and optional default value.


| Modifier | Direction | Mutability | Semantics | Ownership | Initialization |
| -------- | --------- | ---------- | --------- | --------- | -------------- |
| 'ref'    | both      | readwrite  | reference | shared    | by caller      |
| 'out'    | output    | writeonly  | reference | unique    | by callee      |
| 'in'     | input     | readonly   | reference | shared    | by caller      |
| 'copy'   | input     | readwrite  | value     | unique    | by caller      |
| 'move'   | input     | readwrite  | value     | unique    | by caller      |
| 'params' | input     | readonly   | reference | shared    | by caller      |

Grammar:

```
funciton_param_modifier
    : 'in'
    | 'out'
    | 'ref'
    | 'copy'
    | 'move'
    | 'params'
    ;

function_param
    : identifier ':' (function_param_modifier)* type
    ;

function_params
    : function_param (',' function_param)*
    ;

parenthesized_function_params
    : '(' function_params? ')'
    ;
```

## Extend blocks

Extend blocks are used to extend existing types with new functionality.

Functions defined in `extend` block are associated with specified type. It is reachable through type name by using `::` operator.

```
public extend int32
{
    public function GetPapalNumber() -> int32
    {
        return 2137;
    }
}

let funny = int32::GetPapalNumber();
```

Function can be associated with instance of given type by using `self` keyword in place of first argument. It is reachable through `.` operator on instance of given type. Instance is implicitly passed as first argument to function.

```
public extend int32
{
    public function IsEven(self: Self) -> bool
    {
        return self % 2 == 0;
    }
}

let isEven = 42.IsEven();
```

## Structs

Structs are used to define new types.

### Field ordering

Compiler is free to reorder fields of struct to optimize memory layout.

By default, fields are ordered by size in descending order. Fields of same size are ordered by alignment in descending order. Fields of same size and alignment are ordered by order of declaration.

Based on performance profiling, compiler can reorder fields to optimize cache locality.

Compiler can also reorder fields to optimize memory usage. For example, compiler can reorder fields to minimize padding between fields.

```
public struct SomeData
{
    public var a: uint8;
    public var b: uint64;
    public var c: [uint8: 7];
}

assert(typeof(SomeData).Size == 16);
assert(typeof(SomeData).Alignment == 8);
```

### Explicit layout

Struct can be defined as `explicit`, which means that it's fields are defined in specific order. This is useful for defining types that are mapped to memory layout of other types, especially to be compatible with C language.

It is also useful for defining types that are mapped to memory layout of hardware registers.

```
namespace Core.Platform.Linux
{
    public explicit struct timespec
    {
        public var tv_sec: int64;
        public var tv_nsec: int64;
    }
}

namespace Core.Platform.Windows
{
    public explicit struct FILETIME
    {
        public var dwLowDateTime: uint32;
        public var dwHighDateTime: uint32;
    }
}
```

Structs with explicit layout are allowed to be casted to other types with explicit layout. Compiler will generate code to copy fields from one struct to another.

```
namespace Core.Platform.Windows
{
    internal extend FILETIME
    {
        public function ToTicks(self: ref Self) -> int64
        {
            let ticks = builtin(bitcast, self, int64);
            return ticks;
        }
    }
}
```

### Partial structs

Struct can be defined as `partial`, which means that it's fields can be defined in multiple files in same module. This is useful for defining types that are partially generated by code generators. Partial structs cannot be used with explicit layout - there is no way to define order of fields.

## Enums

TODO

## Tuple types

Tuple types are used to define types that are used to group multiple values together. Tuple types are defined by list of types enclosed in parentheses.

```
public type Point = (int32, int32);

public type Vector3 = (X: float32, Y: float32, Z: float32);
```

## Strong types

TODO
IDEA: used to define new type based on existing one. It disallows type conversion between them.

```
public struct MyInt : int32; // 1.
public struct MyInt(int32);  // 2.
public struct MyInt = int32; // 3.
```

## Concepts

Concepts are used to define common traits of types. They model requirements for given type to be used in certain context. Concept can have default implementation for certain functions. Concept can have generic constraints.

Functions can be defined inside a `concept` block. Such functions can have default implementation which can be overriden. Functions without implementation must be overriden.

Example below defines random number generator concept. It defines three functions: `SampleUInt32`, `SampleUInt64` and `SampleBytes`. `SampleUInt64` has default implementation which uses `SampleUInt32` to generate 64-bit random number. `SampleBytes` uses `SampleUInt64` to generate random bytes. Default implementations can be overriden in `extend` block.

```
public concept RandomGenerator
{
    // Function declaration - must be overriden.
    public function SampleUInt32(self: ref Self) -> uint32;

    // Default implementation - may be overriden in extend block.
    public function SampleUInt64(self: ref Self) -> uint64
    {
        return ((self.SampleUInt32() as uint64) << 32) | self.SampleUInt32() as uint64;
    }

    // Default implementation - may be overriden in extend block.
    public function SampleBytes(self: ref Self, buffer: mutable [uint8])
    {
        let blocksCount = buffer.Length / 8;
        let remainder = buffer.Length % 8;

        for i in 0..blocksCount
        {
            let value = self.SampleUInt64().AsBytes(); // returns [uint8: 8] slice array

            for j in 0..8
            {
                buffer[i * 8 + j] = value[j];
            }
        }

        if remainder > 0
        {
            let value = self.SampleUInt64().AsBytes();

            for i in 0..remainder
            {
                buffer[blocksCount * 8 + i] = value[i];
            }
        }
    }
}

public struct Generator
{
    private var state: uint64;
}

public extend Generator as RandomGenerator
{
    public function SampleUInt32(self: ref Self) -> uint32
    {
        self.state = (self.state * 6364136223846793005) + 1;
        return (self.state >> 32) as uint32;
    }
}
```

## Attributes

Attributes can be attached to almost all entities in language.

---
Syntax:

```
balanced-token-sequence
    : token*
    | '{' balanced-token-sequence '}'
    | '(' balanced-token-sequence ')'
    | '[' balanced-token-sequence ']'
    ;

attribute-specifier
    : qualified-identifier
    | qualified-identifier '(' balanced-sequence-of-tokens ')'
    ;

attribute-declaration
    : '[<' attribute-specifier (',' attribute-specifier)* '>]'
    ;
```

---

Example:

```
[<Available(edition: 2022)>]
[<Deprecated("because reasons"), ForceInline>]
public function MyFunction(a: int32, b: int32) => a + b;
```

Alternative syntax:

```
@Available(edition: 2022)
@Deprecated("because reasons") @ForceInline
```

## Contracts

Contracts are mechanism to provide runtime validation of function preconditions and postconditions.

Semantics of contract failure handling:

| semantic | action   | handled | comments |
|----------|----------|---------|-|
| ignore   | continue | no      | |
| assume   | continue | no      | assumes that preconditions are true; this may be removed in final specification |
| observe  | continue | ye      | |
| enforce  | abort    | yes     | |

- function body
  - `assert(expression, ...)` - for source code assertions, with optional formatted error message

- function precondition
    - `in: expression ;`
    - `in { statements... }`
        - contains list of statements which must be a sequence of assertions
    - preconditions must not change state of observed values
- function postcondition
    - `out identifier: expression ;`
    - `out: expression ;`
    - `out (identifier) { statements... }`
    - `out { statements... }`

```weave
function Foo(a: int32, b: int32, out c: int32) -> int32
    // input contracts
    in: a > 42;
    in
    {
        assert((21 <= b) and (b <= 37);
    }
    // output contracts
    out: c > 22;
    out
    {
        assert((a * b) < 2137);
    }

    // return contracts
    out r: r < 666;
    out (r)
    {
        assert(a + r);
    }
{
    return a + b;
}
```

Contract statements are expanded by compiler at call site. Preconditions are checked before function call, postconditions are checked after function call. Compiler is permitted to report possible contradictions between conflicting contracts at compile time. Compiler may assume that preconditions and postconditions are always true, which can impact generated executable.

## Constraints

Constraints are used to define requirements for generic parameters. Constraints can be defined for types, function arguments and return value. Constraints are defined using `where` keyword.

Example below shows generic function with type constraint.

```
public function MultiplyAdd![type T](a: in T, b: in T, c: in T) -> T
    where T: Addable + Multiplicable
{
    return (a * b) + c;
}
```

Constraints can also be used with concepts.

```
public concept NormalDistribution![type T]
{
    where T: Addable + Multiplicable;

    public function Sample(self: ref Self, generator: ref RandomGenerator) -> T;
}
```

## Properties

Properties are fields with explicit setter and getter functions.

```
private var _value: int32;

public var MyProperty: int32 {
    get {
        return self._value;
    }
    set (value) {
        self._value = value;
    }
}
```

Compiler is permitted to choose best fit qualifier for setter value. Value type is infered from type of property.

## Notes

```
public struct Point
{
    public var X: int32;
    public var Y: int32;
}

public struct Size
{
    public var Width: int32;
    public var Height: int32;
}

public struct Vector
{
    public var X: int32;
    public var Y: int32;
}

public enum Option![type T]
{
    None,
    Some(T),
}

public partial struct Object
{
    private var position: Point;
}

// generated code - conditionally compiled into source
#if DEBUG
public partial struct Object
{
    private var debugName: string;
}
#endif
```

Concepts is a way to define functionality that can be implemented by types. Concept may define default implementation for certain functions. Concepts can have generic constraints.

```js
public concept Addable![type Other = Self]
{
    public function Add(self: in Self, other: in Other) -> Self;
}
```

Extend block is used to define functions associated with given type.

```js
public extend Point
{
    public function New(x: int32, y: int32) -> Point
    {
        return Point { X: x, Y: y };
    }
}
```

Extend block can be used to implement concepts for types.

```js
public extend Point as Addable![Vector]
{
    public function Add(self: in Point, other: in Vector) -> Point
    {
        return Point { X: self.X + other.X, Y: self.Y + other.Y };
    }
}
```

## Literals

- string literal `"example"`
- character literal `'a'`
- integer literal `0x1234_5678_u128`
- float literal `21.37_f32`
- array literal
```
let myArray: [int32] = { 2, 1, 3, 7 }

// with indices (may be out-of-order / sparse)
let myArray: [int32] = {
    0: 2,
    1: 1,
    3: 7,
    2: 3,
};
```

- object literal
```
// Explicit types (linter may hint removing ': Vec4' type clause)
let myObject: Vec4 = Vec4{
    X: 2,
    Y: 1,
    Z: 3,
    W: 7,
};

// Typep deduced
let myObject = Vec4{
    X: 2,
    Y: 1,
    Z: 3,
    W: 7,
};
```
- tuple literal `(a, b)`
- labelled tuple literal (type infered from lhs)
```
let x: (a: int32, b: int32) = (b: 21, a: 37);
// question: could this use '{}' as just like other literals?
let y: (a: int32, b: int32) = {b: 21, a: 37};

public var PositiveX: Vector3 => { X: 1, Y: 0, Z: 0 };
public var PositiveZ => Vector3{ X: 0, Y: 0, Z: 1 };
```
- empty unit tuple `()`

## Examples

```js
namespace Core
{
    public enum Result![type T, type E]
    {
        Ok(T),
        Error(E),
    }
}

namespace Core.Platform.Windows
{
    public type HANDLE = native ptr;        // native ptr is a type alias for platform-specific pointer type - in case of C language, it is `void*`
    public type DWORD = uint32;

    public struct FileHandle(HANDLE);
}

namespace Core.IO
{
    // Enum definition
    public enum IoError
    {
        FileNotFound,
        AccessDenied,
        InvalidHandle,
    }

    // Type alias
    public type Result![type T] = Core.Result![T, Core.IO.IoError];

    public concept Reader
    {
        public function Read(self: ref Self, position: int64, buffer: mutable [uint8]) -> Result![usize];
    }

    public concept Writer
    {
        public function Write(self: ref Self, position: int64, buffer: const [uint8]) -> Result![usize]; // Note: `const` keyword is implicit by default, but can be explicitely specified
    }

    public struct FileHandle
    {
        internal var handle: Core.Platform.Windows.FileHandle;
    }

    private const DefaultBufferSize: usize = 16uz << 10uz; // 16 KiB

    public extend FileHandle as Reader
    {
        private function GetOverlappedForPosition(position: int64) -> OVERLAPPED
        {
            // 'uninitialized' is a long keyword, but it might be rarely used. In normal cases, compiler can skip initialization of variable if it is passed to function as out parameter.
            var overlapped: OVERLAPPED = uninitialized;

            overlapped.Offset = position & 0xFFFFFFFF;
            overlapped.OffsetHigh = position >> 32;

            return overlapped;
        }

        public function Read(self: ref Self, position: int64, buffer: mutable [uint8]) -> Result![usize]
        {
            var processed: usize = 0;

            while processed < buffer.Length
            {
                var overlapped = self.GetOverlappedForPosition(position + processed);
                let requested = Min(buffer.Length - processed, DefaultBufferSize);
                var dwProcessed : DWORD;

                if not ReadFile(self.handle, buffer[processed..], requested, &dwProcessed, &overlapped)
                {
                    let dwError = GetLastError();

                    if dwError == ERROR_HANDLE_EOF
                    {
                        return Result::Ok(processed);
                    }

                    if dwError != ERROR_IO_PENDING
                    {
                        return IoError::FromGetLastError(dwError);
                    }

                    if not GetOverlappedResult(self.handle, &overlapped, &dwProcessed, TRUE)
                    {
                        return IoError::FromGetLastError(dwError);
                    }
                }

                processed += dwProcessed;
            }

            return Result::Ok(processed);
        }
    }
}

namespace Core.Numerics
{
    public struct UInt128
    {
        private var lower: uint64;
        private var upper: uint64;
    }

    public struct UInt256
    {
        private var lower: UInt128;
        private var upper: UInt128;
    }

    public extend UInt128
    {
        public function New(lower: uint64, upper: uint64) -> UInt128
        {
            return UInt128 { lower: lower, upper: upper };
        }
    }

    public extend UInt256
    {
        public function New(lower: UInt128, upper: UInt128) -> UInt256
        {
            return UInt256 { lower: lower, upper: upper };
        }
    }

    public extend UInt128 as Addable
    {
        public function Add(self: UInt128, other: UInt128) -> UInt128
        {
            var lower = self.lower + other.lower;
            var upper = self.upper + other.upper;

            if lower < self.lower
            {
                upper += 1;
            }

            return UInt128::New(lower, upper);
        }
    }

    public extend UInt256 as Addable
    {
        public function Add(self: UInt256, other: UInt256) -> UInt256
        {
            var lower = self.lower + other.lower;
            var upper = self.upper + other.upper;

            if lower < self.lower
            {
                upper += 1;
            }

            return UInt256::New(lower, upper);
        }
    }
}

// Introduce type alias in global namespace
public type uint128 = UInt128;

namespace Core.Collections
{
    public concept Enumerator![type T]
    {
        public function MoveNext(self: ref Self) -> bool;
        public function Current(self: ref Self) -> ref T;
    }

    public concept Enumerable![type T]
    {
        public function GetEnumerator(self: Self) -> Enumerator![T];
    }

    public struct List![type T]
    {
        private var buffer: [T];
        private var count: usize;
    }

    public extend List![type T] as Enumerable![T]
    {
        public function GetEnumerator(self: List![T]) -> Enumerator![T]
        {
            return ListEnumerator![T] { list: self, index: 0 };
        }
    }

    public struct ListEnumerator![type T]
    {
        private var list: ref List![T];
        private var index: usize;
    }

    public extend ListEnumerator![type T] as Enumerator![T]
    {
        public function MoveNext(self: ref ListEnumerator![T]) -> bool
        {
            if self.index < self.list.count
            {
                self.index += 1;
                return true;
            }

            return false;
        }

        public function Current(self: ref ListEnumerator![T]) -> ref T
            where self.index < self.list.count
        {
            return self.list.buffer[self.index];
        }
    }
}
```
