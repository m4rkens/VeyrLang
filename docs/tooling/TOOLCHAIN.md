# Veyr Toolchain Strategy

**Status:** Initial accepted tooling direction

## 1. Tool names

| Tool | Role |
|---|---|
| `veyr` | primary project/build/package CLI |
| `veyrc` | compiler frontend/driver |
| `veyrcore` | internal compiler library used by tools |
| `veyr-lsp` | language server |
| `veyrfmt` | formatter |
| `veyr-analyze` | standalone analyzer / analyzer mode frontend |
| `veyr-doc` | documentation generator |
| `veyr-bindgen` | C/native binding generator |
| `veyr-gpu-bindgen` | future GPU API binding generator/probe tool |
| `veyr-test` | test-runner interface; likely surfaced as `veyr test` |
| `veyr-pkg` | package registry/package-management interface; likely surfaced through `veyr` |

## 2. File names

| Name | Purpose |
|---|---|
| `.vyr` | Veyr source file |
| `Veyr.toml` | package/project/build manifest |
| `Veyr.lock` | resolved dependency lockfile |
| `.veyr/` | local cache/build metadata directory |

## 3. Primary CLI shape

The `veyr` command is the user-facing workflow tool:

```text
veyr init
veyr check
veyr build
veyr run
veyr test
veyr fmt
veyr analyze
veyr doc
veyr clean
veyr graph
veyr bindgen
veyr package
```

The `veyrc` command is lower-level and compiler-oriented:

```text
veyrc --check path/to/root.vyr
veyrc --emit=obj path/to/root.vyr
veyrc --emit=llvm-ir path/to/root.vyr
veyrc --target x86_64-linux-gnu path/to/root.vyr
```

## 4. Driver layering

```text
veyr CLI
  -> manifest/project resolver
  -> build graph planner
  -> veyrc invocations
  -> linker/native tools
  -> test/doc/analyze/fmt wrappers
```

`veyrc` should not need to understand registry workflows or package publishing. `veyr` owns project orchestration.

## 5. Analyzer integration

The static analyzer is not an optional afterthought. It should become part of the normal `veyr check` and `veyr build` flow.

Early phases may expose analyzer checks through compiler diagnostics. Later phases may support:

```text
veyr analyze --explain
veyr analyze --json
veyr analyze --sarif
```

## 6. LSP direction

`veyr-lsp` should use `veyrcore` rather than duplicating parser/semantic logic. It should support recovered parse/lex output for editor diagnostics.

## 7. Binding tools

`veyr-bindgen` should initially target C headers and simple native libraries.

`veyr-gpu-bindgen` is a future/probe tool for large API surfaces such as Vulkan/OpenGL/DX12/Metal. It should not be started before the FFI model, repr rules, function pointer model, and raw/safe wrapper layering are understood.


## Cross-platform tooling requirement

All Veyr tools are expected to remain viable on Windows and Linux. macOS is a future Tier 2 target.

Tool behavior involving paths, process spawning, cache directories, native libraries, terminal output, generated bindings, or platform SDKs must use explicit platform handling rather than hardcoded assumptions.
