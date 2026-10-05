"""Native exact-config/static-Support/runtime probe; no language implementation."""

import os
import shlex

from conan import ConanFile
from conan.errors import ConanInvalidConfiguration
from conan.tools.build import can_run
from conan.tools.cmake import CMake, CMakeDeps, CMakeToolchain, cmake_layout


class LLVMTestConan(ConanFile):
    settings = "os", "arch", "compiler", "build_type"
    test_type = "explicit"

    def requirements(self):
        self.requires(self.tested_reference_str)

    def layout(self):
        cmake_layout(self, generator="Ninja")

    def generate(self):
        deps = CMakeDeps(self)
        deps.generate()
        toolchain = CMakeToolchain(self, generator="Ninja")
        toolchain.user_presets_path = False
        if self.settings.os == "Windows":
            runtime = "MultiThreadedDebugDLL" if self.settings.compiler.runtime_type == "Debug" else "MultiThreadedDLL"
            toolchain.cache_variables["VEYR_EXPECTED_MSVC_RUNTIME"] = runtime
        toolchain.generate()

    def build(self):
        cmake = CMake(self)
        cmake.configure()
        cmake.build()

    def test(self):
        if not can_run(self):
            raise ConanInvalidConfiguration("This native runtime probe must run; cross compilation is deferred")
        executable = os.path.join(self.cpp.build.bindir, "llvm_package_probe")
        if self.settings.os == "Windows":
            executable += ".exe"
            command = '"' + executable.replace('"', '\\"') + '"'
        else:
            command = shlex.quote(executable)
        self.run(command, env="conanrun")
