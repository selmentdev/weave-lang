# Language Syntax

## Overview

Syntax is based on `specifier name : type = initializer` pattern

## Functions

```
public function Add(self: int32, other: int32) -> int32 = { ... }
```

## Types

User defined types are supported as structs, tuples, interfaces and traits.

Structs can have fields, properties and methods defined.

```
struct Empty { };

struct Vector3 {
    public field X : float32 = 21.37f32;

    private field _y : float32 = default;
};

struct Strong(int32);

struct NamedTuple(int32, int32, float32);

// Unnamed tuple (note that `struct` keyword is ommited).
(float32, int32);
```

## Aliases

Any entity can have alias.

```
// Unnamed tuples can be "named" through aliases.
public alias Vector3 = (float32, float32, float32);

public function Consume(x: float32, float32, float32)) -> float32;
```

## Initializers

Explicit default (zero) initialization:

```
var x : float32 = default;
```

Explicit uninitialized storage:

```
var x : float32 = void;

// reading from x before assignment is compilation error
// let y = x;
x = 32.0f;
let y = x;
```

## Arrays

Arrays define storage for entities of given type. It does not perform initialization on it's own.

```
// Bound array
public alias Vector3 = [float32: 3];

// Unbound array
public alias Floats = [float32];
```

Bound array can be passed by unbound array - length is propagated as (pointer, size).

```
let x : [float32: 4] = default; // default initialize array of 4 floats, using automatic storage
// transpiled to `float32_t x[4] = { 0.0f, 0.0f, 0.0f, 0.0f, };`

let y : [float32] = x;
// transpiled to `array_view<float32_t> y = { x, 4u };
```

## Properties

```
public struct Vector3 {
    public field X : float32;
    public field Y : float32;
    private field _z : float32;

    // C# like syntax
    public property Length : float32 {
        get => ...
    }

    public property Z : float32 {
        get => self._z;                 // implicit `self`
        set => self._z = value;         // implicit `value`
    }

    // Explciit getter/setter
        public getter Length(self: Vector3) : float32 { ... }
        public getter Z(self: Vector3) : float32 => self._z;
        public setter Z(self: Vector3, value: float32) => self._z = value;

    // Explicit property
        public property Length(self: Vector3) : float32 { ... }
        public property Z(self: Vector3) : float32 { ... }
        public property Z(self: Vector3, value: float32) { ... }

    // Magic function syntax
        public function Length(self: Vector3) : float32 { ... }
        public function Z(self: Vector3) : float32 { ... }
        public function Z(self: Vector3, value: float32) { ... }
}

Vector3 v;

let x = v.Length; // `()` can be ommited for properties / any function with just `self` parameter
```

## Named constructor pattern

```
public struct Vector4 {
    public field X : float32;
    public field Y : float32;
    public field Z : float32;
    public field W : float32;

    public function New(x: float32, y: float32, z: float32, w: float32) : Vector4 {
        
    }
}
```
