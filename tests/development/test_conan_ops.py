"""Mocked orchestration checks; no package/network/native build commands."""

import contextlib
from dataclasses import replace
import io
import os
from pathlib import Path
import subprocess
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "scripts"))
import dev
from devtools import conan_ops, doctor
from devtools.doctor import BaselineError, make_selection, select_inputs
from test_doctor import LINUX, WINDOWS, matching_observations, reviewed_fixture


class ConanOperationsFixture:
    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        self.directory = Path(self.temporary.name)
        self.root = self.directory / "Checkout with spaces"
        self.root.mkdir()
        self.baseline = reviewed_fixture()
        self.selected = select_inputs(self.baseline, LINUX, root=self.root)
        with patch.object(doctor, "_probe", return_value=str(self.root / ".git")):
            self.selection = make_selection(self.baseline, self.selected, root=self.root,
                                            conan_home=self.directory / "Package home")
        self.selection.conan_home.mkdir()
        self.selection.conan_executable.parent.mkdir(parents=True)
        self.selection.conan_executable.touch()
        (self.root / "conanfile.py").write_text("# fixture\n")
        for path in {*self.selected["host_profiles"], *self.selected["build_profiles"]}:
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text("# profile fixture\n")
        recipe = self.root / "conan/recipes/llvm"
        recipe.mkdir(parents=True)
        (recipe / "conanfile.py").write_text("# fixture\n")
        self.selection.lockfile.parent.mkdir(parents=True)
        self.selection.lockfile.write_text('{"version":"0.5","requires":[]}\n')
        self.calls = []

    def run_child(self, argv, **kwargs):
        self.calls.append((argv, kwargs))
        if argv[1:3] == ["lock", "create"]:
            target = next(arg.split("=", 1)[1] for arg in argv if arg.startswith("--lockfile-out="))
            Path(target).write_text('{"version":"0.5","requires":[]}\n')
        return subprocess.CompletedProcess(argv, 0)

    def invoke(self, function, selection=None, selected=None, run=None, **kwargs):
        with patch.object(conan_ops.subprocess, "run", side_effect=run or self.run_child), \
             contextlib.redirect_stdout(io.StringIO()):
            function(selection or self.selection, selected or self.selected, root=self.root, **kwargs)


class ConanOperationsTests(ConanOperationsFixture, unittest.TestCase):
    def test_setup_is_explicit_missing_build_and_cached_probe_only(self):
        before = self.selection.lockfile.read_bytes()
        self.invoke(conan_ops.setup_dependencies)
        self.assertEqual([call[0][1] for call in self.calls], ["export", "install", "test"])
        install, probe = self.calls[1][0], self.calls[2][0]
        self.assertIn("--build=missing", install)
        self.assertNotIn("--no-remote", install)
        self.assertIn("&:llvm_provider=managed", install)
        self.assertIn(f"--output-folder={self.selection.build_dir}", install)
        self.assertIn("--build=never", probe)
        self.assertIn("--no-remote", probe)
        self.assertIn("llvm/23.1.2@veyr/m1", probe)
        self.assertNotIn("&:llvm_provider=managed", probe)
        self.assertEqual(self.selection.lockfile.read_bytes(), before)
        for argv, kwargs in self.calls:
            self.assertEqual(argv[0], str(self.selection.conan_executable))
            self.assertEqual(kwargs["env"]["CONAN_HOME"], str(self.selection.conan_home))
            self.assertEqual(kwargs["cwd"], self.root)
            self.assertTrue(kwargs["check"])
            self.assertNotIn("shell", kwargs)

    def test_profiles_spaced_paths_and_native_build_configuration_are_structured(self):
        self.invoke(conan_ops.setup_dependencies)
        install = self.calls[1][0]
        for kind, paths in (("h", self.selected["host_profiles"]), ("b", self.selected["build_profiles"])):
            actual = [install[index + 1] for index, arg in enumerate(install) if arg == f"-pr:{kind}"]
            self.assertEqual(actual, [str(path) for path in paths])
        self.assertEqual(self.selected["build_configuration"], "Release")
        self.assertIn(str(self.root / "conanfile.py"), install)

    def test_prepare_has_no_remote_no_build_no_bootstrap_or_lock_update(self):
        before = self.selection.lockfile.read_bytes()
        with patch.object(dev, "bootstrap_conan") as bootstrap:
            self.invoke(conan_ops.prepare_dependencies)
        bootstrap.assert_not_called()
        self.assertEqual(len(self.calls), 1)
        argv = self.calls[0][0]
        self.assertIn("--no-remote", argv)
        self.assertIn("--build=never", argv)
        self.assertNotIn("--build=missing", argv)
        self.assertEqual(argv[1], "install")
        self.assertEqual(self.selection.lockfile.read_bytes(), before)

    def test_missing_cache_failure_stops_once_without_acquisition(self):
        def fail(argv, **kwargs):
            self.calls.append((argv, kwargs))
            raise subprocess.CalledProcessError(1, argv)
        with self.assertRaisesRegex(BaselineError, "exit=1.*No retry"):
            self.invoke(conan_ops.prepare_dependencies, run=fail)
        self.assertEqual(len(self.calls), 1)
        self.assertIn("--no-remote", self.calls[0][0])
        self.assertIn("--build=never", self.calls[0][0])
        self.assertFalse(self.selection.conan_home.with_name("Package home.veyr-operation.lock").exists())

    def test_missing_lock_requires_explicit_initialization_and_prepare_never_creates_it(self):
        self.selection.lockfile.unlink()
        for operation in (conan_ops.setup_dependencies, conan_ops.prepare_dependencies):
            with self.subTest(operation=operation), self.assertRaisesRegex(BaselineError, "--init-lock"):
                self.invoke(operation)
        self.assertEqual(self.calls, [])
        self.assertFalse(self.selection.lockfile.exists())
        self.invoke(conan_ops.setup_dependencies, initialize_lock=True)
        self.assertEqual([call[0][1] for call in self.calls], ["export", "lock", "install", "test"])
        self.assertTrue(self.selection.lockfile.is_file())
        self.assertIn("--lockfile=", self.calls[1][0])
        self.assertIn("&:llvm_provider=managed", self.calls[1][0])

    def test_explicit_initialization_never_overwrites_existing_lock(self):
        before = self.selection.lockfile.read_bytes()
        self.invoke(conan_ops.setup_dependencies, initialize_lock=True)
        self.assertNotIn("lock", [argv[1] for argv, _ in self.calls])
        self.assertEqual(self.selection.lockfile.read_bytes(), before)

    def test_external_providers_use_same_native_lock_without_managed_probe(self):
        llvm = self.directory / "External LLVM with spaces"
        llvm.mkdir()
        (llvm / "LLVMConfig.cmake").touch()
        for provider in ("system", "custom"):
            with self.subTest(provider=provider):
                self.calls = []
                selection = replace(self.selection, provider=provider, llvm_dir=llvm,
                                    build_dir=self.root / "build" / provider / "Debug")
                self.invoke(conan_ops.setup_dependencies, selection)
                self.assertEqual([argv[1] for argv, _ in self.calls], ["install"])
                self.assertIn(f"&:llvm_provider={provider}", self.calls[0][0])
                self.assertIn(f"--lockfile={selection.lockfile}", self.calls[0][0])
                self.calls = []
                self.invoke(conan_ops.prepare_dependencies, selection)
                self.assertEqual(len(self.calls), 1)
                self.assertIn("--no-remote", self.calls[0][0])

    def test_external_provider_missing_config_fails_before_any_command(self):
        selection = replace(self.selection, provider="custom", llvm_dir=self.directory / "missing")
        with self.assertRaisesRegex(BaselineError, "LLVMConfig.cmake; no fallback"):
            self.invoke(conan_ops.setup_dependencies, selection)
        self.assertEqual(self.calls, [])

    def test_home_and_output_boundaries_fail_without_running_conan(self):
        for path in (self.root, self.root / "cache", Path.home() / ".conan2"):
            with self.subTest(path=path), self.assertRaises(BaselineError):
                self.invoke(conan_ops.setup_dependencies, replace(self.selection, conan_home=path))
        with self.assertRaisesRegex(BaselineError, "output must be"):
            self.invoke(conan_ops.setup_dependencies, replace(self.selection, build_dir=self.root))
        self.assertEqual(self.calls, [])

    def test_absent_home_prepare_is_read_only_and_does_not_initialize(self):
        home = self.directory / "Absent home"
        with self.assertRaisesRegex(BaselineError, "Missing prepared Conan home"):
            self.invoke(conan_ops.prepare_dependencies, replace(self.selection, conan_home=home))
        self.assertFalse(home.exists())
        self.assertFalse(home.with_name(home.name + ".veyr-operation.lock").exists())
        self.assertEqual(self.calls, [])

    def test_conflicting_operations_fail_without_retry_or_marker_removal(self):
        with conan_ops.conan_home_guard(self.selection, self.root):
            marker = self.selection.conan_home.with_name("Package home.veyr-operation.lock")
            before = marker.read_bytes()
            with self.assertRaisesRegex(BaselineError, "Conflicting Conan operation"):
                self.invoke(conan_ops.setup_dependencies)
            self.assertEqual(marker.read_bytes(), before)
        self.assertFalse(marker.exists())
        self.assertEqual(self.calls, [])

    def test_full_setup_prerequisite_callback_runs_under_guard_before_dependencies(self):
        order = []
        def prerequisites():
            self.assertTrue(self.selection.conan_home.with_name("Package home.veyr-operation.lock").is_file())
            self.assertEqual(self.calls, [])
            order.extend(["bootstrap", "doctor"])
        self.invoke(conan_ops.setup_dependencies, ensure_prerequisites=prerequisites)
        self.assertEqual(order, ["bootstrap", "doctor"])
        self.assertEqual(len(self.calls), 3)

    def test_setup_prerequisite_or_install_failure_has_no_retry_or_probe(self):
        def prerequisites():
            raise BaselineError("fixture prerequisite mismatch")
        with self.assertRaisesRegex(BaselineError, "prerequisite mismatch"):
            self.invoke(conan_ops.setup_dependencies, ensure_prerequisites=prerequisites)
        self.assertEqual(self.calls, [])
        def fail_install(argv, **kwargs):
            self.calls.append((argv, kwargs))
            if argv[1] == "install":
                raise subprocess.CalledProcessError(7, argv)
            return subprocess.CompletedProcess(argv, 0)
        with self.assertRaisesRegex(BaselineError, "exit=7"):
            self.invoke(conan_ops.setup_dependencies, run=fail_install)
        self.assertEqual([argv[1] for argv, _ in self.calls], ["export", "install"])

    def test_missing_profile_or_project_owned_executable_cannot_use_path_fallback(self):
        self.selected["host_profiles"][0].unlink()
        with self.assertRaisesRegex(BaselineError, "Missing reviewed profile"):
            self.invoke(conan_ops.prepare_dependencies)
        self.selected["host_profiles"][0].touch()
        self.selection.conan_executable.unlink()
        with patch.dict(os.environ, {"PATH": "/unrelated/conan"}), \
             self.assertRaisesRegex(BaselineError, "Expected project-owned Conan"):
            self.invoke(conan_ops.prepare_dependencies)
        self.assertEqual(self.calls, [])

    def test_configs_and_providers_have_distinct_build_dirs_and_expected_lock(self):
        selections = []
        for config in ("Debug", "Release"):
            selected = select_inputs(self.baseline, LINUX, config, root=self.root)
            with patch.object(doctor, "_probe", return_value=str(self.root / ".git")):
                selection = make_selection(self.baseline, selected, root=self.root,
                                            conan_home=self.selection.conan_home)
            selections.append(selection)
            self.assertEqual(selection.lockfile.name, f"{LINUX}-{config}.lock")
        self.assertNotEqual(selections[0].build_dir, selections[1].build_dir)
        self.assertEqual(selections[0].conan_home, selections[1].conan_home)
        selections[1].lockfile.touch()
        with self.assertRaisesRegex(BaselineError, "Unexpected lock selection"):
            self.invoke(conan_ops.prepare_dependencies, replace(self.selection, lockfile=selections[1].lockfile))

    def test_configure_missing_plan04_files_stops_before_conan_or_cmake(self):
        with self.assertRaisesRegex(BaselineError, "until Plan 01-04"):
            self.invoke(conan_ops.configure_project)
        self.assertEqual(self.calls, [])

    def test_configure_prepares_then_supplies_child_only_paths_and_cache_inputs(self):
        (self.root / "CMakeLists.txt").touch()
        (self.root / "CMakePresets.json").touch()
        python = self.directory / "Reviewed Python/python"
        before = dict(os.environ)
        self.invoke(conan_ops.configure_project, python=python)
        self.assertEqual(len(self.calls), 2)
        conan, cmake = self.calls
        self.assertIn("--no-remote", conan[0])
        self.assertIn("--build=never", conan[0])
        self.assertEqual(cmake[0][:3], ["cmake", "--preset", f"{LINUX}-Debug"])
        self.assertEqual(cmake[1]["env"]["VEYR_BUILD_DIR"], str(self.selection.build_dir))
        self.assertEqual(cmake[1]["env"]["VEYR_CONAN_TOOLCHAIN"],
                         str(self.selection.build_dir / "generators/conan_toolchain.cmake"))
        self.assertEqual(cmake[1]["env"]["VEYR_PYTHON"], str(python))
        self.assertIn("-DVEYR_LLVM_PROVIDER=managed", cmake[0])
        self.assertIn("-DVEYR_LLVM_DIR=", cmake[0])
        self.assertEqual(dict(os.environ), before)

    def test_failed_prepare_cannot_reach_cmake(self):
        (self.root / "CMakeLists.txt").touch()
        (self.root / "CMakePresets.json").touch()
        def fail(argv, **kwargs):
            self.calls.append((argv, kwargs))
            raise subprocess.CalledProcessError(1, argv)
        with self.assertRaisesRegex(BaselineError, "exit=1"):
            self.invoke(conan_ops.configure_project, run=fail)
        self.assertEqual(len(self.calls), 1)
        self.assertEqual(self.calls[0][0][0], str(self.selection.conan_executable))

    def test_windows_fixture_uses_absolute_exe_native_overlays_and_separate_cache(self):
        selected = select_inputs(self.baseline, WINDOWS, root=self.root)
        with patch.object(doctor, "_probe", return_value=str(self.root / ".git")):
            selection = make_selection(self.baseline, selected, root=self.root,
                                      conan_home=self.directory / "Windows cache with spaces")
        selection.conan_home.mkdir()
        selection.conan_executable.parent.mkdir(parents=True)
        selection.conan_executable.touch()
        selection.lockfile.touch()
        for path in {*selected["host_profiles"], *selected["build_profiles"]}:
            path.parent.mkdir(parents=True, exist_ok=True)
            path.touch()
        self.invoke(conan_ops.prepare_dependencies, selection, selected)
        argv, kwargs = self.calls[0]
        self.assertTrue(argv[0].endswith("Scripts/conan.exe"))
        self.assertEqual(kwargs["env"]["CONAN_HOME"], str(selection.conan_home))
        self.assertEqual(kwargs["env"]["VEYR_PROFILE_OS"], "Windows")
        self.assertEqual(selected["runtime_type"], "Debug")
        self.assertEqual(selected["build_runtime_type"], "Release")


class DeveloperSetupDispatchTests(ConanOperationsFixture, unittest.TestCase):
    # Exercise dispatch while all external tools and observations stay mocked.
    def cli(self, args):
        with patch.object(dev, "read_baseline", return_value=self.baseline), \
             patch.object(dev, "make_selection", return_value=self.selection), \
             patch.object(dev.platform, "system", return_value="Linux"), \
             patch.object(dev.platform, "machine", return_value="x86_64"), \
             patch.dict(os.environ, {}, clear=True), \
             contextlib.redirect_stdout(io.StringIO()), contextlib.redirect_stderr(io.StringIO()):
            return dev.main(args)

    def test_tools_only_does_not_dispatch_dependency_setup(self):
        with patch.object(dev, "bootstrap_conan", return_value="reused") as bootstrap, \
             patch.object(dev, "setup_dependencies") as setup, \
             patch.object(dev, "collect_observations") as observe:
            result = self.cli(["setup", "--tools-only"])
        self.assertEqual(result, 0)
        bootstrap.assert_called_once()
        setup.assert_not_called()
        observe.assert_not_called()

    def test_full_setup_bootstraps_before_doctor_and_dependency_children(self):
        order = []
        def setup(selection, selected, **kwargs):
            self.assertFalse(kwargs["initialize_lock"])
            kwargs["ensure_prerequisites"]()
            order.append("dependencies")
        with patch.object(dev, "bootstrap_conan", side_effect=lambda *args: order.append("bootstrap") or "reused"), \
             patch.object(dev, "collect_observations", side_effect=lambda *args: order.append("doctor") or matching_observations(self.baseline, LINUX)), \
             patch.object(dev, "setup_dependencies", side_effect=setup):
            result = self.cli(["setup"])
        self.assertEqual(result, 0)
        self.assertEqual(order, ["bootstrap", "doctor", "dependencies"])

    def test_configure_validates_without_bootstrapping_or_setup(self):
        with patch.object(dev, "bootstrap_conan") as bootstrap, \
             patch.object(dev, "setup_dependencies") as setup, \
             patch.object(dev, "collect_observations", return_value=matching_observations(self.baseline, LINUX)), \
             patch.object(dev, "configure_project") as configure:
            result = self.cli(["configure"])
        self.assertEqual(result, 0)
        bootstrap.assert_not_called()
        setup.assert_not_called()
        configure.assert_called_once()


if __name__ == "__main__":
    unittest.main()
