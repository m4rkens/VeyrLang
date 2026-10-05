"""Narrow M1 LLVM source package; acquisition/cache/identity stay Conan-owned."""

from pathlib import Path

from conan import ConanFile
from conan.errors import ConanException, ConanInvalidConfiguration
from conan.tools.build import check_min_cppstd
from conan.tools.cmake import CMake, CMakeToolchain, cmake_layout
from conan.tools.files import copy, get


class LLVMConan(ConanFile):
    name = "llvm"
    version = "23.1.2"
    user = "veyr"
    channel = "m1"
    package_type = "static-library"
    license = "Apache-2.0 WITH LLVM-exception"
    url = "https://github.com/llvm/llvm-project"
    description = "M1 X86 static LLVM with upstream native CMake exports"
    settings = "os", "arch", "compiler", "build_type"
    options = {"fPIC": [True, False]}
    default_options = {"fPIC": True}

    def config_options(self):
        if self.settings.os == "Windows":
            del self.options.fPIC

    def validate(self):
        if str(self.version) != "23.1.2":
            raise ConanInvalidConfiguration("This narrow recipe supports LLVM 23.1.2 only")
        if self.settings.os not in ("Linux", "Windows") or self.settings.arch != "x86_64":
            raise ConanInvalidConfiguration("M1 supports native Windows/Linux x86_64; macOS is deferred")
        if self.settings.compiler != "clang" or self.settings.build_type != "Release":
            raise ConanInvalidConfiguration("Use the reviewed Clang profile with llvm/*:build_type=Release")
        check_min_cppstd(self, "23")
        if self.settings.os == "Linux" and self.settings.compiler.libcxx != "libstdc++11":
            raise ConanInvalidConfiguration("Linux requires the reviewed libstdc++11 ABI")
        if self.settings.os == "Windows" and (
            self.settings.compiler.runtime != "dynamic"
            or self.settings.compiler.runtime_version != "v143"
        ):
            raise ConanInvalidConfiguration("Windows requires reviewed v143 dynamic CRT settings")

    def layout(self):
        # Extract the monorepo intact: llvm needs its cmake/ and libc/ siblings.
        cmake_layout(self, generator="Ninja", src_folder="src")

    def source(self):
        get(self, **self.conan_data["sources"][str(self.version)], strip_root=True,
            retry=0)

    def generate(self):
        toolchain = CMakeToolchain(self, generator="Ninja")
        toolchain.user_presets_path = False
        toolchain.cache_variables.update({
            "CMAKE_INSTALL_LIBDIR": "lib",
            "BUILD_SHARED_LIBS": False,
            "LLVM_TARGETS_TO_BUILD": "X86",
            "LLVM_ENABLE_PROJECTS": "",
            "LLVM_ENABLE_RUNTIMES": "",
            "LLVM_BUILD_LLVM_DYLIB": False,
            "LLVM_LINK_LLVM_DYLIB": False,
            "LLVM_BUILD_LLVM_C_DYLIB": False,
            "LLVM_ENABLE_ASSERTIONS": False,
            "LLVM_ABI_BREAKING_CHECKS": "FORCE_OFF",
            "LLVM_INCLUDE_TOOLS": False,
            "LLVM_INCLUDE_UTILS": False,
            "LLVM_INCLUDE_RUNTIMES": False,
            "LLVM_INCLUDE_TESTS": False,
            "LLVM_INCLUDE_EXAMPLES": False,
            "LLVM_INCLUDE_BENCHMARKS": False,
            "LLVM_INCLUDE_DOCS": False,
            "LLVM_ENABLE_BINDINGS": False,
            "LLVM_ENABLE_TELEMETRY": False,
            "LLVM_ENABLE_ZLIB": False,
            "LLVM_ENABLE_ZSTD": False,
            "LLVM_ENABLE_LIBXML2": False,
            "LLVM_ENABLE_FFI": False,
            "LLVM_ENABLE_LIBEDIT": False,
            "LLVM_ENABLE_LIBPFM": False,
            "LLVM_ENABLE_CURL": False,
            "LLVM_ENABLE_HTTPLIB": False,
            "LLVM_ENABLE_ICU": False,
            "LLVM_ENABLE_ICONV": False,
            "LLVM_ENABLE_Z3_SOLVER": False,
        })
        # LLVM 23.1.2 exposes no LLVM_ENABLE_TERMINFO; do not pass unused flags.
        if self.settings.os == "Windows":
            toolchain.cache_variables["LLVM_ENABLE_DIA_SDK"] = False
            if self.settings.compiler.runtime_type == "Debug":
                # Optimized /MDd must retain Debug STL ABI; never force it to zero.
                toolchain.preprocessor_definitions["_ITERATOR_DEBUG_LEVEL"] = 2
        toolchain.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure(build_script_folder="llvm")
        cmake.build()

    def package(self):
        CMake(self).install()
        copy(self, "LICENSE.TXT", src=str(Path(self.source_folder) / "llvm"),
             dst=str(Path(self.package_folder) / "licenses"))
        native = Path(self.package_folder) / "lib" / "cmake" / "llvm"
        for filename in ("LLVMConfig.cmake", "LLVMConfigVersion.cmake", "LLVMExports.cmake"):
            if not (native / filename).is_file():
                raise ConanException(f"Required native LLVM export missing: {native / filename}")

    def package_info(self):
        self.cpp_info.set_property("cmake_find_mode", "none")
        self.cpp_info.builddirs = ["lib/cmake/llvm"]
        # Default Conan package identity retains compiler/libcxx/CRT/runtime_type.
        # Upstream CMake exports own component libraries and their system links.
