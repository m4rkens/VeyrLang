# Validation Project Cards

These are intentionally small projects used to force Veyr language/compiler/tooling capabilities. They are not product promises and must not be implemented before their prerequisite milestones.

## audio

- [Beat Metadata Manager](audio/AUDIO_01_BEAT_METADATA_MANAGER.md)
- [Plugin Preset Library](audio/AUDIO_02_PLUGIN_PRESET_LIBRARY.md)
- [Realtime Audio Buffer Probe](audio/AUDIO_03_REALTIME_AUDIO_BUFFER_PROBE.md)

## cli

- [Argument Echo CLI](cli/CLI_01_ARG_ECHO.md)
- [Calculator CLI](cli/CLI_02_CALCULATOR.md)
- [wc-like Text Counter](cli/CLI_03_WC_TOOL.md)
- [grep-like Search Tool](cli/CLI_04_GREP_TOOL.md)
- [Todo CLI with Persistence](cli/CLI_05_TODO_CLI.md)
- [Config Parser](cli/CLI_06_CONFIG_PARSER.md)
- [Veyr.toml Parser Subset](cli/CLI_07_PACKAGE_MANIFEST_PARSER.md)

## code-analysis

- [Mini Source Tokenizer](code-analysis/CODE_01_SOURCE_TOKENIZER.md)
- [AST Graph Exporter](code-analysis/CODE_02_AST_GRAPH_EXPORTER.md)
- [CFG/DDG Explorer](code-analysis/CODE_03_CFG_DDG_EXPLORER.md)
- [Vulnerability Pattern Scanner](code-analysis/CODE_04_VULNERABILITY_PATTERN_SCANNER.md)

## concurrency

- [Multithreaded File Hasher](concurrency/CONC_01_MULTITHREADED_FILE_HASHER.md)
- [Job System Prototype](concurrency/CONC_02_JOB_SYSTEM.md)

## ffi

- [C API Wrapper](ffi/FFI_01_C_API_WRAPPER.md)
- [Dynamic Library Loader](ffi/FFI_02_DYNAMIC_LIBRARY_LOADER.md)

## graphics

- [Native Window](graphics/GFX_01_NATIVE_WINDOW.md)
- [Software Framebuffer](graphics/GFX_02_SOFTWARE_FRAMEBUFFER.md)
- [OpenGL Clear Color](graphics/GFX_03_OPENGL_CLEAR.md)
- [Vulkan Loader](graphics/GFX_04_VULKAN_LOADER.md)
- [Vulkan Device and Surface](graphics/GFX_05_VULKAN_DEVICE_SURFACE.md)
- [Vulkan Swapchain Clear](graphics/GFX_06_VULKAN_SWAPCHAIN_CLEAR.md)
- [Vulkan Triangle](graphics/GFX_07_VULKAN_TRIANGLE.md)
- [Cross-API GPU Core v0](graphics/GFX_08_CROSS_API_GPU_CORE.md)

## memory

- [Arena Allocator](memory/MEM_01_ARENA_ALLOCATOR.md)
- [Resource Wrapper Suite](memory/MEM_02_RESOURCE_WRAPPER_SUITE.md)
- [Linear Allocator](memory/MEM_03_LINEAR_ALLOCATOR.md)
- [Ownership Stress Test](memory/MEM_04_OWNERSHIP_STRESS_TEST.md)

## modules

- [Static Library plus Executable](modules/MOD_01_STATICLIB_PLUS_EXE.md)
- [Shared Library Plugin Host](modules/MOD_02_PLUGIN_HOST.md)
- [Platform System Info](modules/MOD_03_PLATFORM_SYSTEM_INFO.md)

## network

- [Minimal Blocking HTTP Server](network/NET_01_MINIMAL_HTTP_SERVER.md)

## platform

- [Platform Path and Filesystem Cases](platform/PLAT_01_PATHS_FILESYSTEM_CASES.md)
- [Process, Exit Code, Args, and Environment Probe](platform/PLAT_02_PROCESS_EXIT_ARGS_ENV.md)
- [Terminal and Diagnostics Portability Probe](platform/PLAT_03_TERMINAL_DIAGNOSTICS.md)
- [Dynamic Library Suffix and Loader Probe](platform/PLAT_04_DYNAMIC_LIBRARY_SUFFIXES.md)
- [Socket Platform Shim Probe](platform/PLAT_05_SOCKET_PLATFORM_SHIM.md)
- [Windows/Linux CI Matrix Probe](platform/PLAT_06_CI_PLATFORM_MATRIX.md)
- [macOS Feasibility Notes Probe](platform/PLAT_07_MACOS_FEASIBILITY_NOTES.md)

## security

- [Sysmon Log Filter](security/SEC_01_SYSMON_LOG_FILTER.md)
- [MITRE Rule Matcher](security/SEC_02_MITRE_RULE_MATCHER.md)
- [Atomic Red Team Result Parser](security/SEC_03_ATOMIC_RESULTS_PARSER.md)
- [Sigma-like Rule Subset](security/SEC_04_SIGMA_SUBSET.md)

## self-host

- [Mini Language in Veyr](self-host/LANG_01_MINI_LANGUAGE.md)
- [Veyr Lexer in Veyr](self-host/LANG_02_VEYR_LEXER_IN_VEYR.md)
- [Source and Diagnostics in Veyr](self-host/LANG_03_SELF_HOSTING_SOURCE_DIAGNOSTICS.md)

## simracing

- [Simracing Telemetry Parser](simracing/SIM_01_TELEMETRY_PARSER.md)
- [Pedal Calibration CLI](simracing/SIM_02_PEDAL_CALIBRATION_CLI.md)
- [Force-Feedback Profile Tool](simracing/SIM_03_FFB_PROFILE_TOOL.md)
- [Binary Protocol Codec](simracing/SIM_04_BINARY_PROTOCOL_CODEC.md)

## tooling

- [veyrfmt Probe](tooling/TOOL_01_VEYR_FMT.md)
- [veyr-lsp Diagnostics Probe](tooling/TOOL_02_VEYR_LSP_DIAGNOSTICS.md)
- [Veyr Test Runner](tooling/TOOL_03_TEST_RUNNER.md)
- [Package Manager Probe](tooling/TOOL_04_PACKAGE_MANAGER.md)
- [C Binding Generator Probe](tooling/TOOL_05_BINDGEN_C.md)
- [GPU Binding Generator Probe](tooling/TOOL_06_GPU_BINDGEN.md)
