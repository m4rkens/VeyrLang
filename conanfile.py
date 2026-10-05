"""Compiler implementation dependencies; Conan owns resolution and package state."""

from conan import ConanFile
from conan.tools.cmake import CMakeDeps, CMakeToolchain


class VeyrDependencies(ConanFile):
    settings = "os", "arch", "compiler", "build_type"
    options = {"llvm_provider": ["managed", "system", "custom"]}
    default_options = {
        "llvm_provider": "managed",
        "gtest/*:build_gmock": True,
        "gtest/*:shared": False,
    }

    def requirements(self):
        self.requires("gtest/1.18.0")
        if self.options.llvm_provider == "managed":
            self.requires("llvm/23.1.2@veyr/m1")

    def layout(self):
        # Conan's selected output folder is the build root for this consumer.
        self.folders.build = "."
        self.folders.generators = "generators"

    def generate(self):
        toolchain = CMakeToolchain(self, generator="Ninja")
        toolchain.user_presets_path = None
        toolchain.generate()
        CMakeDeps(self).generate()
