# `Veyr.toml` Project Manifest and Build-System Direction

**Status:** Initial accepted project-system direction  
**Manifest name:** `Veyr.toml`  
**Lockfile:** `Veyr.lock`  
**Primary CLI:** `veyr`

## 1. Purpose

`Veyr.toml` is the package, target, dependency, profile, platform, and build orchestration manifest for Veyr projects.

It should replace ad-hoc build scripts for Veyr language projects, but it does not replace the early CMake build used to implement the compiler in C++.

## 2. Design goals

The project system must make these visible:

- package identity;
- source roots;
- target kinds;
- root modules;
- output artifact type;
- dependencies and versions;
- feature flags;
- platform-specific dependencies;
- native libraries and link options;
- build profiles;
- generated bindings;
- shader/resource pipelines in graphics projects;
- test and benchmark targets.

It must avoid hidden magic and global runtime assumptions.

## 3. Initial manifest sketch

```toml
[package]
name = "example"
version = "0.1.0"
edition = "2026"
license = "Apache-2.0"

[workspace]
members = ["tools/*", "libs/*"]

[paths]
source = "src"
tests = "tests"
resources = "resources"

[targets.app]
kind = "executable"
root = "src/main.vyr"

[targets.core]
kind = "staticlib"
root = "src/core/lib.vyr"

[dependencies]
# package = "^1.2"
# local = { path = "../local-package" }

[features]
default = []
trace = []

[profiles.dev]
opt-level = 0
debug = true
analyze = true
overflow-checks = true

[profiles.release]
opt-level = 3
debug = false
analyze = true
overflow-checks = true
lto = "thin"

[platform.'cfg(os = "windows")'.native]
libs = ["user32", "kernel32"]

[platform.'cfg(os = "linux")'.native]
libs = ["dl", "pthread"]

[platform.'cfg(os = "macos")'.native]
frameworks = ["Cocoa", "Metal", "QuartzCore"]
libs = []
```

## 4. Target kinds

Initial target kinds:

- `executable`;
- `staticlib`;
- `sharedlib`;
- `plugin`;
- `test`;
- `bench`;
- `example`;
- `tool`.

The target's root module determines the module graph used for compilation.

## 5. Native linking

Native dependencies must be explicit. The build system should not infer important native links silently.

Examples:

```toml
[targets.gpu_demo.native]
libs = ["vulkan"]
include-dirs = []
link-dirs = []
```

## 6. Generated bindings

Binding generation should be explicit and reproducible:

```toml
[bindgen.vulkan]
tool = "veyr-gpu-bindgen"
input = "extern/vulkan/vk.xml"
output = "generated/vulkan"
api = "vulkan"
mode = "raw"
```

Generated code must be written to configured output paths and should not be mixed into hand-written source without clear boundaries.

## 7. Graphics/shader resources

Future graphics projects may need explicit resource pipelines:

```toml
[resources.shaders]
input = "shaders"
output = ".veyr/generated/shaders"
compiler = "glslang"
profiles = ["dev", "release"]
```

This is a planned direction, not an early Milestone 1 requirement.

## 8. Build commands

Expected CLI mapping:

```text
veyr check       # parse/semantic/analyzer checks
veyr build       # compile selected targets
veyr run         # build/run executable target
veyr test        # build/run test targets
veyr doc         # generate docs
veyr graph       # show module/build graph
veyr clean       # remove build/cache outputs
```

## 9. Non-goals for early milestones

Early milestones must not implement a full package registry, remote dependency resolver, cross-compilation matrix, shader pipeline, or lockfile solver before language basics and module artifacts exist.


## 10. Cross-platform project-system requirements

Windows and Linux are mandatory Tier 1 platforms. macOS is a Tier 2 future platform.

`Veyr.toml` must eventually represent platform-sensitive configuration explicitly:

- native libraries;
- frameworks on macOS;
- link directories;
- generated bindings;
- output artifact names/suffixes;
- platform-specific source roots/modules;
- platform-specific resources;
- target availability.

Platform conditionals are build/module selection rules, not a textual preprocessor.

Example:

```toml
[targets.window_demo]
kind = "executable"
root = "src/window_demo/main.vyr"
platforms = ["windows", "linux"]

[platform.'cfg(os = "windows")'.targets.window_demo.native]
libs = ["user32", "gdi32"]

[platform.'cfg(os = "linux")'.targets.window_demo.native]
libs = ["X11", "dl", "pthread"]

[platform.'cfg(os = "macos")'.targets.window_demo.native]
frameworks = ["Cocoa", "QuartzCore", "Metal"]
```

The project system must distinguish "not supported yet" from "unsupported by design".

## 11. Full reference example

A maximal, commented reference manifest is available at:

```text
docs/project-system/Veyr.full.example.toml
```

The same reference is mirrored as an example-project shape at:

```text
examples/project-configs/full-reference/Veyr.toml
```

This file intentionally shows many future-facing manifest sections in one place, including package metadata, workspace layout, targets, dependencies, features, profiles, analyzer settings, formatter/LSP/test settings, resource pipelines, bindgen configuration, native linking, platform-specific target settings, and future research platform gates.

It is documentation, not an implementation checklist. The presence of a field in the full reference example does not authorize an agent to implement that feature in the current milestone. New manifest fields become implementation work only when a milestone plan explicitly justifies them through a design probe and the owner approves that scope.
