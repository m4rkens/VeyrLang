# Graphics / Windowing / GPU API Probe Track

**Status:** Long-term validation track  
**Purpose:** force real FFI, native handles, resource ownership, binary layout, and GPU API abstractions through small projects.

## 1. Principle

Do not start with a giant abstraction over Vulkan/OpenGL/DX12/Metal.

Start with tiny probes:

```text
window -> framebuffer -> dynamic loading -> OpenGL clear -> Vulkan instance -> Vulkan swapchain clear -> triangle -> shared abstraction
```

Only extract common abstractions after real backend pressure exists.

## 2. GFX-01 — Minimal native window

Goal:

- create a native window;
- run event loop;
- close cleanly.

Forces:

- platform modules;
- WinAPI/X11/Wayland/SDL-like abstraction decisions;
- opaque native handles;
- event tagged unions;
- resource cleanup;
- `Result` errors.

## 3. GFX-02 — Window + software framebuffer

Goal:

- draw CPU-side pixels;
- present buffer;
- gradient/rectangle/debug overlay.

Forces:

- byte buffers;
- slices;
- fixed-size arrays;
- pixel formats;
- stride/pitch;
- endian/layout awareness.

## 4. GFX-03 — Dynamic native library loading

Goal:

- `LoadLibrary` / `dlopen`;
- `GetProcAddress` / `dlsym`;
- typed symbol wrappers.

Forces:

- dynamic library handles;
- raw function pointers;
- nullable raw pointers;
- safe wrappers;
- lifetime dependency between library and symbol.

## 5. GFX-04 — OpenGL loader probe

Goal:

- create OpenGL context;
- load minimal functions;
- clear screen color;
- swap buffers.

Forces:

- platform-specific context creation;
- runtime symbol resolution;
- raw function pointers;
- context-bound/thread-affine resources.

## 6. GFX-05 — Vulkan loader probe

Goal:

- load Vulkan loader;
- fetch `vkGetInstanceProcAddr`;
- create `VkInstance`;
- enumerate physical devices;
- destroy instance.

Forces:

- C structs;
- `repr(C)`;
- pointers;
- nullable pointers;
- function pointers;
- flags/enums;
- Result mapping;
- allocator callbacks as optional/null.

## 7. GFX-06 — Vulkan surface + device selection

Goal:

- window;
- Vulkan instance;
- surface;
- physical device selection;
- logical device;
- queue.

Forces:

- platform-specific extensions;
- native window handle interop;
- arrays/slices passed to C;
- temporary buffers;
- ownership of handles.

## 8. GFX-07 — Vulkan swapchain clear

Goal:

- swapchain;
- acquire image;
- command buffer;
- clear color;
- present.

Forces:

- resource dependency graph;
- destruction order;
- fences/semaphores;
- command pool/buffer ownership;
- synchronization errors.

## 9. GFX-08 — Vulkan triangle

Goal:

- shader modules;
- vertex buffer;
- pipeline;
- draw triangle.

Forces:

- binary shader file loading;
- alignment;
- nested C structs;
- arrays of structs;
- flags;
- layout decisions.

## 10. GFX-09 — Vulkan thin safe wrapper

Goal:

- raw binding remains faithful;
- safe layer wraps ownership and errors.

Layering:

```text
raw Vulkan FFI
-> typed handles
-> Result-returning functions
-> Drop-owned resources
-> builder helpers
```

## 11. GFX-10 — Cross-API graphics abstraction v0

Goal:

- common concepts over OpenGL/Vulkan first;
- leave DX12/Metal planned until platform/tooling pressure exists.

Common concepts candidates:

- `BufferUsage`;
- `TextureFormat`;
- `ShaderStage`;
- `PresentMode`;
- `CommandBuffer`;
- `Swapchain`;
- `Device`;
- `Queue`.

Non-goal:

- hiding all backend differences;
- pretending Vulkan/OpenGL/DX12/Metal are the same;
- building an engine before binding and resource semantics are tested.

## 12. Future backend direction

- OpenGL: useful for loader/context probes and simple backend.
- Vulkan: primary explicit modern graphics pressure test.
- DX12: later Windows/backend probe.
- Metal: future Apple backend, likely through a separate platform layer.

## 13. Why this track matters for Veyr

This track pressures:

- FFI;
- `repr(C)`;
- raw pointers;
- function pointers;
- nullable pointers;
- unsafe blocks;
- Drop;
- defer;
- Result;
- tagged unions;
- flags/enums;
- newtypes;
- byte buffers;
- slices;
- allocators;
- platform modules;
- dynamic libraries;
- build/link config;
- opaque handles;
- thread-affine resources.

## Project cards

Smaller validation projects for this track are split into dedicated cards under:

```text
docs/design/tracks/projects/
```

Use those cards when planning milestones so the track develops through small concrete programs rather than one oversized project.
