# Memory, Ownership, Borrowing, and Cleanup Model

**Status:** accepted design direction with provisional syntax.

## 1. Goals

The memory model should provide:

- manual and explicit control;
- deterministic cleanup;
- unique ownership as the default owning abstraction;
- non-owning references whose lifetime is analyzed;
- low-level raw pointers for FFI and systems work;
- no mandatory garbage collector;
- no requirement to annotate ordinary code with explicit lifetimes;
- diagnostics that explain provenance and invalidation.

The language must not equate manual memory management with unsafe code.

## 2. Conceptual categories

The following spellings are conceptual and may change.

```text
T             ordinary inline value
Owner<T>      unique owner of an indirectly stored resource
Ref<T>        immutable non-owning reference
MutRef<T>     mutable non-owning reference
Slice<T>      immutable borrowed contiguous range
MutSlice<T>   mutable borrowed contiguous range
Shared<T>     shared ownership; library-level and planned
*T            raw pointer/address
```

### 2.1 Ordinary values

`T` follows value semantics.

A value may be:

- Copy;
- move-only;
- Drop-requiring;
- zero-sized;
- address-sensitive only if a future pinning mechanism is used.

Inline value construction never implies heap allocation.

### 2.2 Unique ownership

Unique ownership is a core semantic model.

A unique owner:

- owns one resource;
- cannot be copied accidentally;
- can transfer ownership by move;
- performs deterministic cleanup when its lifetime ends.

The exact surface type spelling is open.

### 2.3 Borrowed references

`Ref<T>` and `MutRef<T>` are non-null analyzable borrows.

A reference should carry enough semantic information for the analyzer to track:

- origin/provenance;
- backing resource identity;
- storage location when relevant;
- lifetime dependencies;
- invalidation events.

A borrowed reference is not a raw pointer with nicer syntax.

### 2.4 Raw pointers

Raw pointers represent low-level addresses.

They may:

- be null;
- point to invalid or released memory;
- come from FFI;
- lack ownership information;
- lack a statically proven lifetime.

Storing or passing a raw pointer need not be unsafe. Operations that rely on its validity, especially dereference and unchecked arithmetic, require an unsafe boundary.

## 3. Copy, move, clone, and drop

### 3.1 Copy

Copy is permitted only for types whose duplication is:

- semantically obvious;
- predictably cheap;
- safe to perform implicitly.

A simple scalar or small value type may be Copy.

A large heap-backed buffer should not silently duplicate hundreds of megabytes because of ordinary assignment.

### 3.2 Clone

Potentially expensive or non-trivial duplication should be explicit through a named clone-like operation.

```text
let image2 := image1.clone();
```

The exact trait/API remains open, but the semantic separation between implicit cheap copy and explicit potentially expensive duplication is accepted.

### 3.3 Move

Ownership transfer between live named values must be visible.

Conceptual example:

```text
let destination := move source;
```

After the move, `source` is statically unavailable unless a future feature defines a safe reinitialization path.

Moves may be inferred when the source is already ending its lifetime, for example:

- returning a local value;
- passing a temporary;
- consuming a temporary expression.

The goal is explicit ownership transition without useless ceremony.

### 3.4 Drop

Types that own resources participate in deterministic cleanup.

Drop-like cleanup:

- occurs at lifetime end;
- follows a defined order;
- must not propagate a second error while a scope is already exiting;
- is distinct from explicit close/flush operations that may report failure.

## 4. `defer`

`defer` is a core local-cleanup mechanism.

It is not a replacement for Drop.

Use Drop when cleanup belongs to the type. Use defer when cleanup belongs to a particular control-flow region.

Examples of appropriate defer use:

- a small foreign C API handle before a safe wrapper exists;
- restoring temporary process or renderer state;
- rollback of a local side effect;
- low-level lock/unlock pair;
- local temporary file removal.

Accepted semantics:

1. defer belongs to the current lexical scope;
2. it is registered only if control reaches its declaration;
3. it runs on controlled scope exit;
4. controlled exits include ordinary end of scope, return, break, continue, and typed error propagation;
5. actions run in LIFO order;
6. captured bindings use normal ownership and borrowing semantics;
7. defer does not silently snapshot or copy values;
8. deferred use extends the value's required lifetime;
9. a deferred cleanup action cannot propagate a second error out of an already exiting scope.

Conceptual example:

```text
let handle := foreign_create();

defer {
    foreign_destroy(handle);
}
```

## 5. `errdefer`

`errdefer` is a strong candidate, not yet a fully frozen core feature.

Its intended role is rollback, not ordinary resource cleanup.

It should run when the current scope exits through a typed recoverable-error path, such as:

- `return Err(...)`;
- a `?`-style propagated error.

It should not run on ordinary success.

The current direction is that panic is a separate category and does not count as an errdefer error exit.

## 6. Unified cleanup order

The design direction is one conceptual cleanup stack containing:

- automatic Drop actions;
- defer actions;
- errdefer actions.

Actions are ordered according to resource creation and defer registration, then executed in reverse order. Conditional actions such as errdefer are skipped when their condition is not met.

The exact lowering must be validated through design probes.

## 7. Lifetime inference

Explicit lifetime annotations must not be required for ordinary programming.

The default strategy is:

- infer lifetime dependencies;
- track borrows to their last possible use;
- analyze control flow;
- store inferred public borrow contracts in module metadata.

Conceptual example:

```text
func first<T>(items: Slice<T>) -> Ref<T> {
    return items[0];
}
```

The compiler should infer that the returned reference originates from `items`.

## 8. Non-lexical lifetime analysis

A borrow ends at its last possible use, not automatically at the closing brace.

```text
let mut user := User(...);
let name := ref user.name;

print(name);

// `name` is no longer used.
user.name = "Kuba";
```

This should be accepted when the analyzer can prove the borrow has ended.

## 9. Provenance and resource identity

The analyzer should distinguish:

- variable/binding identity;
- resource identity;
- storage location.

This matters when ownership moves but the underlying allocation does not.

Conceptual example:

```text
let owner := allocate<User>();
let view := ref owner.value;
let other := move owner;

print(view);
```

This may remain valid if the underlying resource identity and storage location are unchanged.

By contrast, moving an inline object may invalidate references to its previous storage location.

## 10. Borrowing rules

The accepted direction is hybrid rather than purely lexical.

Fundamental rule:

- many immutable borrows may coexist;
- overlapping mutable access must be exclusive.

The type system should reject obvious contradictions. The analyzer should use flow-sensitive and field-sensitive reasoning for more precise cases.

### 10.1 Partial borrowing

Distinct fields should be borrowable independently when proven disjoint.

```text
let health := mut ref player.health;
let inventory := ref player.inventory;
```

### 10.2 Disjoint mutable slices

Provably non-overlapping mutable regions should be allowed.

```text
let left := values[0..50];
let right := values[50..100];
```

The initial analyzer may support only simple statically obvious region splits, but the architecture should permit greater precision later.

## 11. Invalidation effects

Some operations preserve borrows; others may invalidate them.

For example, a dynamic vector push may reallocate backing storage.

The semantic interface of an API may need metadata such as:

```text
may_reallocate(self)
invalidates(element_borrows(self))
invalidates(slices(self))
```

Exact syntax is open. The capability is core.

Downstream modules must be able to analyze invalidation without reopening full source text.

## 12. Safe indexing

A safe slice or array access cannot produce UB.

Direction:

- if bounds are proven at compile time: no runtime check;
- otherwise: runtime bounds check;
- failure: defined panic;
- explicit unchecked access: unsafe.

A separate fallible API should provide Option- or Result-like access when out-of-range is part of ordinary control flow.

## 13. Shared ownership

Shared ownership is planned as a library abstraction, not the language's foundational ownership model.

A `Shared<T>`-like abstraction may use reference counting.

Important separation:

```text
shared ownership != shared mutability
```

Examples:

```text
Shared<Config>          // many owners of immutable data
Shared<Mutex<State>>    // synchronized shared mutation
Shared<Atomic<Counter>> // atomic shared mutation
```

## 14. Pinning

A future `Pin<T>`-like mechanism may be required for:

- self-referential structures;
- intrusive containers;
- async state machines;
- some foreign APIs.

It remains a candidate until real probes demonstrate necessity.

## 15. Unsafe boundaries

Unsafe is required for operations whose correctness depends on facts the compiler cannot prove.

Examples:

- dereferencing raw pointers;
- constructing a safe reference from raw memory without proof;
- unchecked pointer arithmetic;
- arbitrary object reinterpretation;
- bypassing lifetime or ownership rules;
- declaring a foreign wrapper thread-safe without compiler-verifiable evidence.

Unsafe should be local and auditable.

A preferred pattern is:

```text
small unsafe implementation core
    -> validated safe abstraction
    -> ordinary safe application code
```

## 16. Diagnostics requirements

Memory diagnostics should explain relationships, not only report internal compiler states.

A good invalidated-view diagnostic should show:

1. where the view originated;
2. which resource backed it;
3. where that resource was released or invalidated;
4. where the invalid use occurs;
5. possible fixes.

A good moved-value diagnostic should show:

1. where ownership transfer occurred;
2. why the value can no longer be used;
3. whether borrowing, cloning, or changing control flow may solve the issue.
