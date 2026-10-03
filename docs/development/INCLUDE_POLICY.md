# Include Policy for Veyr-Owned C++ Code

**Status:** Required implementation policy

## 1. Canonical include style

All Veyr-owned headers must be included with angle brackets:

```cpp
#include <veyr/source/source_span.hpp>
#include <veyr/diagnostics/diagnostic.hpp>
```

Do not use:

```cpp
#include "source_span.hpp"
#include "../source/source_span.hpp"
#include "/absolute/path/to/header.hpp"
#include "veyr/source/source_span.hpp"
```

## 2. Header layout

Public/internal project headers should live under a canonical include root:

```text
include/veyr/...
```

Source files should include them through that root.

## 3. Enforcement

The build should provide an include-style guardrail/check target and a CTest entry.

The check should fail for:

- quoted includes of Veyr-owned headers;
- relative include paths;
- source-directory-only include assumptions;
- inconsistent casing.

## 4. Third-party exception

Third-party headers may follow their own include style. Do not rewrite external dependency includes unless they are under Veyr ownership.
