# Type System, Functions, Traits, and Compile-Time Model

**Status:** accepted design direction with provisional syntax.

# 1. Type-system goals

The type system should be:

- static;
- strong;
- explicit at public boundaries;
- locally ergonomic through inference;
- integrated with ownership and borrowing;
- capable of zero-cost nominal abstraction;
- expressive enough for system APIs, generic libraries, tagged unions, and high-performance code;
- resistant to accidental implicit conversions.

## 2. Primitive types

Accepted direction:

```text
int8 int16 int32 int64 int128
uint8 uint16 uint32 uint64 uint128
isize usize
float32 float64
bool
byte
char
void
never
```

### 2.1 Integer literals

Integer literals are compile-time values before contextual typing.

```text
let value := 10;
```

Without context, the initial default direction is `int32`.

```text
let value: uint8 = 10;
```

is allowed because the compiler can prove the literal fits.

```text
let value: uint8 = 300;
```

is a compile-time error.

### 2.2 Floating literals

The default direction for unconstrained floating literals is `float64`.

### 2.3 Bool

Bool is not an integer.

The following are invalid in ordinary code:

```text
if 10 { ... }
let flag: bool = 1;
```

Explicit comparison is required.

### 2.4 Byte

`byte` is semantically distinct from `uint8`.

```text
Slice<byte>
```

represents raw data.

```text
Slice<uint8>
```

represents numeric values.

The layout may be identical; the semantics are not.

### 2.5 Char

`char` should represent a Unicode scalar value, not an 8-bit byte.

### 2.6 Void and never

`void` is a real zero-size type with one state.

`never` is the type of expressions that do not return normally, such as panic and abort.

## 3. Structs and tuples

Structs are product types.

```text
struct User {
    id: UserId;
    name: String;
}
```

Struct construction does not imply heap allocation.

Tuples are core, primarily for:

- local grouping;
- destructuring;
- small anonymous returns.

Meaningful public domain data should generally prefer named structs.

## 4. Arrays, slices, and vectors

Fixed-size arrays are core and own their elements.

Conceptual form:

```text
Array<T, N>
```

Slices borrow contiguous storage.

Dynamic vectors are standard-library types and major design probes for:

- allocators;
- growth;
- invalidation;
- ownership;
- generics;
- Drop;
- slices.

## 5. Nullability and absence

Ordinary values and safe references are non-null.

Absence is represented by a tagged-union Option-like type.

```text
Option<User>
Option<Ref<User>>
```

Raw pointers may be null.

The `null` literal belongs to raw-pointer/FFI semantics, not the ordinary object model.

## 6. Type aliases and newtypes

### 6.1 Alias

A type alias is another name for the same type.

```text
type UserIdentifier = uint64;
```

### 6.2 Newtype

A newtype introduces a distinct nominal type with the same representation as its underlying type.

```text
newtype UserId = uint64;
newtype ProductId = uint64;
```

They are not interchangeable.

A newtype is expected to be zero-cost in representation.

It must not automatically inherit every operation from the underlying type merely because the bits are the same.

This is essential for:

- identifiers;
- units;
- OS handles;
- protocol values;
- domain types.

## 7. Conversions

The design separates four categories.

### 7.1 Implicit safe widening

Allowed only when non-lossy, non-failing, and semantically unsurprising.

Examples under consideration:

```text
int8 -> int16 -> int32 -> int64
uint8 -> uint16 -> uint32 -> uint64
float32 -> float64
```

Signed/unsigned changes, integer/float conversion, narrowing, and newtype conversion are not implicit.

### 7.2 Explicit checked conversion

Used when conversion may fail because the target range is smaller or semantics may reject the value.

### 7.3 Explicit lossy conversion

The programmer chooses the behavior, such as:

- truncate;
- round;
- floor;
- ceil;
- wrap;
- saturate.

### 7.4 Bit reinterpretation

A defined size-compatible bit cast is separate from arbitrary raw-memory reinterpretation.

Arbitrary reinterpretation belongs to unsafe low-level code.

## 8. Tagged unions

Tagged unions are the primary enum and variant mechanism.

```text
enum Token {
    Identifier(String),
    Integer(int64),
    Plus,
    Error {
        message: String,
        span: SourceSpan
    }
}
```

Variants may contain:

- no payload;
- positional payload;
- named fields.

The compiler always knows which variant is active in safe code.

### 8.1 Option and Result

Option and Result are conceptually ordinary generic tagged unions:

```text
enum Option<T> {
    Some(T),
    None
}
```

```text
enum Result<T, E> {
    Ok(T),
    Err(E)
}
```

The compiler may provide special ergonomics such as must-use and `?`-style propagation, but their data semantics remain ordinary tagged unions.

### 8.2 Recursive types

Direct infinite-size recursion is rejected.

Recursive structures require indirection through an owning or borrowing abstraction.

### 8.3 Representation

Normal tagged-union layout is compiler-defined.

Explicit representation contracts are required for FFI, binary protocols, shared memory, and other stable-layout cases.

The compiler may use niche optimizations when they do not change semantics.

## 9. Pattern matching

Pattern matching is core.

Requirements:

- exhaustive by default;
- match is an expression;
- nested patterns are supported;
- destructuring integrates with ownership and borrowing;
- a wildcard is explicit;
- missing variants produce diagnostics;
- pattern guards are planned.

Conceptual example:

```text
let value := match result {
    Ok(value) => value,
    Err(error) => panic(error)
};
```

The `never` type allows the panic branch to coexist with a value-producing branch.

### 9.1 Ownership-aware matching

Matching by value may consume the matched value or move payloads out of it.

Borrowed matching should be supported without consumption.

Partial moves should be tracked field-sensitively.

## 10. Arithmetic semantics

Ordinary integer overflow should have one defined behavior across build modes: panic.

Explicit alternatives include:

- wrapping operations;
- saturating operations;
- checked operations;
- overflowing operations if justified.

Division by zero follows the same principle: defined panic in ordinary arithmetic, explicit checked alternative for fallible control flow.

## 11. Function model

Function boundaries use explicit parameter and return types.

Local values may use inference.

Conceptual distinctions:

```text
T          value semantics
Ref<T>     immutable borrow
MutRef<T>  mutable borrow
*T         raw address
```

Named live move-only values require visible ownership transfer.

Conceptual call forms:

```text
inspect(ref data);
modify(mut ref data);
consume(move data);
```

The exact grammar is not frozen.

### 11.1 Argument evaluation

Arguments evaluate left to right.

### 11.2 Return values

A function may return:

- an owned value;
- a borrowed reference;
- an Option-like value;
- a Result-like value;
- a tuple or struct.

Borrow dependencies are inferred and stored in module metadata.

### 11.3 Overloading

Function overloading is core.

Rules:

- no overloading by return type alone;
- exact match is preferred;
- ambiguity is an error;
- overload resolution must not depend on long implicit-conversion chains;
- ownership/borrow mode is part of the callable contract.

## 12. Methods and receivers

Methods are functions with explicit receiver semantics.

Conceptually:

```text
self: Ref<Self>     observe
self: MutRef<Self>  mutate
self: Self          consume
```

Borrowing method receivers may be ergonomically inferred at method call sites.

Consuming a live named receiver should still require visible ownership transfer.

Construction uses ordinary named functions rather than a special constructor hierarchy.

## 13. Closures and callable types

Function pointers and closures are distinct.

A function pointer is code only.

A closure is code plus captured state.

Capture modes should be explicit:

- copy;
- immutable borrow;
- mutable borrow;
- move/ownership.

Creating a closure must not silently heap-allocate.

A closure should be representable as a normal anonymous value when possible.

The callable capability model should distinguish:

- repeatable immutable call;
- repeatable mutable call;
- one-shot consuming call.

This is essential for job systems and ownership-carrying callbacks.

## 14. Object model

The accepted object model separates concerns:

```text
struct -> data
impl -> behavior
trait -> contract/capability
composition -> state reuse
trait defaults / delegation -> behavior reuse
```

Class/state inheritance is not part of the core design.

### 14.1 Traits

Traits are nominal and explicitly implemented.

They may define:

- required methods;
- associated types;
- associated constants;
- default methods;
- refined trait requirements.

Traits do not contain instance state.

### 14.2 Static dispatch

Generic code constrained by traits uses static dispatch by default.

### 14.3 Dynamic dispatch

Dynamic dispatch is explicit through a `dyn Trait`-like mechanism.

It must not silently allocate.

A borrowed dynamic interface may conceptually contain:

- data pointer;
- dispatch-table pointer.

Owned type erasure should be possible over an existing allocation without an extra hidden allocation when representation allows it.

Not every trait must support dynamic dispatch. Diagnostics must explain why a trait is incompatible.

### 14.4 Coherence

Global trait implementations must avoid conflicts.

Direction:

- local trait + foreign type: allowed;
- foreign trait + local type: allowed;
- foreign trait + foreign type: not globally allowed.

A newtype can adapt a foreign type when needed.

## 15. Generics

Generic functions and types are core.

Generic code is validated against declared constraints before instantiation.

This explicitly rejects accidental C++-template duck typing.

A generic body may only use operations guaranteed by:

- the unconstrained language model;
- explicit trait constraints.

Ownership rules fully apply to generic types.

### 15.1 Monomorphization

The default implementation model is monomorphization.

The compiler may merge equivalent generated code internally as an optimization.

### 15.2 Conditional implementations

Methods and trait implementations may exist only when generic constraints are satisfied.

### 15.3 Associated types

Associated types are part of the core design.

Example concept:

```text
trait Iterator {
    type Item;
    func next(self: MutRef<Self>) -> Option<Self.Item>;
}
```

### 15.4 Const generics

Types may depend on compile-time values.

```text
Array<T, N>
Matrix<T, Rows, Columns>
```

The initial supported value kinds should be conservative.

### 15.5 Deferred generic features

The following are not initial requirements:

- higher-kinded types;
- unrestricted specialization;
- generic associated types;
- arbitrary compile-time object parameters.

## 16. Compile-time programming

Compile-time programming uses the same language rather than a separate template language.

The model has three layers.

### 16.1 Const evaluation

Core capability for:

- constants;
- array sizes;
- const generic arguments;
- lookup tables;
- compile-time configuration.

Compile-time execution is deterministic by default.

### 16.2 Typed introspection

Planned capability to inspect:

- fields;
- field types;
- tagged-union variants;
- trait implementations;
- layout properties;
- selected language metadata.

Compile-time introspection should not require runtime reflection data.

### 16.3 Structured code generation

Derive-style generation is planned on top of typed introspection.

Hygienic macros remain a later candidate.

A textual C-style preprocessor is a non-goal.

## 17. Operators

Operators use a predefined, controlled customization system.

Potentially customizable operator families include:

- arithmetic;
- comparison;
- indexing.

The language does not permit inventing arbitrary new operator symbols.

Ownership and control-flow operations are not overloadable:

- move;
- borrow/reference creation;
- assignment semantics;
- logical short-circuit control flow;
- error propagation;
- defer.

Newtypes do not inherit operators merely from representation.

## 18. Diagnostics requirements

A generic constraint failure should explain:

- which type failed;
- which capability is required;
- where the requirement came from;
- which implementation is missing.

A pattern-match diagnostic should explain:

- which variants are missing;
- whether a wildcard is hiding variants.

A conversion diagnostic should distinguish:

- impossible conversion;
- narrowing conversion;
- lossy conversion requiring an explicit mode;
- newtype mismatch.
