"""Acceptance boundaries in temporary trees; all native/package commands mocked."""

import contextlib
import configparser
import copy
from dataclasses import replace
import io
import json
import os
from pathlib import Path
import re
import subprocess
import sys
import tempfile
import textwrap
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "scripts"))
import check_foundation as foundation
import dev
from devtools.doctor import BaselineError
from test_conan_ops import ConanOperationsFixture
from test_doctor import matching_observations, LINUX


def cache_fixture(package_revision="package-revision"):
    return {"Local Cache": {"llvm/23.1.2@veyr/m1": {"revisions": {
        "recipe-revision": {"packages": {"package-id": {
            "revisions": {package_revision: {"timestamp": 1}}}}}}}}}


class FoundationContractTests(ConanOperationsFixture, unittest.TestCase):
    def setUp(self):
        super().setUp()
        self.addCleanup(patch.stopall)
        patch("sys.stdout", io.StringIO()).start()
        patch("sys.stderr", io.StringIO()).start()
        (self.root / "CMakeLists.txt").touch()
        (self.root / "CMakePresets.json").touch()
        policy = self.root / "scripts/devtools/baseline.json"
        policy.parent.mkdir(parents=True)
        policy.write_text(json.dumps(self.baseline))
        self.llvm = self.directory / "Installed LLVM config with spaces"
        self.llvm.mkdir()
        (self.llvm / "LLVMConfig.cmake").write_text("# fixture only; not native evidence\n")
        self.harness = foundation.FoundationChecks(self.selection, self.selected, self.baseline,
                                                   root=self.root, external_dir=self.llvm)
        patch.object(foundation, "collect_observations",
                     return_value=matching_observations(self.baseline, LINUX)).start()
        patch.object(dev, "run_command", side_effect=self.native_fixture).start()
        patch.object(foundation, "run_command", side_effect=self.native_fixture).start()
        self.setup = patch.object(dev, "setup_dependencies").start()
        self.bootstrap = patch.object(dev, "bootstrap_conan").start()

    def native_fixture(self, argv, **kwargs):
        self.calls.append((argv, kwargs))
        if argv[:2] == ["cmake", "--preset"]:
            directory = Path(kwargs["env"]["VEYR_BUILD_DIR"])
            directory.mkdir(parents=True, exist_ok=True)
            llvm = kwargs["env"]["VEYR_LLVM_DIR"] or str(self.llvm)
            (directory / "CMakeCache.txt").write_text(f"LLVM_DIR:PATH={llvm}\n")
            for name in ("build.ninja", "CTestTestfile.cmake"):
                (directory / name).touch()
            (directory / "tests").mkdir()
            (directory / "tests/CTestTestfile.cmake").write_text('add_test([=[include-policy]=] "python")\n')
            (directory / "compile_commands.json").write_text(json.dumps([{
                "directory": str(directory), "file": str(self.root / "example.cpp"),
                "command": self.selected["environment"]["VEYR_CPP_COMPILER"] + " -std=c++23 -c example.cpp"}]))
        elif argv[:2] == ["cmake", "--build"]:
            for name in ("veyrc", "tests/foundation_tests", "tests/llvm_link_tests"):
                (Path(argv[2]) / name).touch()
        elif argv[1:2] == ["list"]:
            path = Path(next(value.split("=", 1)[1] for value in argv if value.startswith("--out-file=")))
            path.write_text(json.dumps(cache_fixture()))
        return subprocess.CompletedProcess(argv, 0)

    def assert_no_setup(self):
        self.setup.assert_not_called()
        self.bootstrap.assert_not_called()
        for argv, kwargs in self.calls:
            if argv[0] == str(self.selection.conan_executable) and argv[1] == "install":
                self.assertIn("--no-remote", argv)
                self.assertIn("--build=never", argv)
                self.assertNotIn("--build=missing", argv)
                self.assertIn(f"--lockfile={self.selection.lockfile}", argv)
                self.assertEqual(kwargs["env"]["CONAN_HOME"], str(self.selection.conan_home))

    def test_prepared_uses_isolated_output_compilation_database_and_one_ctest(self):
        before = foundation.input_identity(self.selection, self.selected, self.root)
        self.harness.execute("prepared")
        self.assertEqual([argv[0] for argv, _ in self.calls],
                         [str(self.selection.conan_executable), "cmake", "cmake", "ctest"])
        self.assert_no_setup()
        output = self.harness.context().selection.build_dir
        self.assertIn(self.root / "build/foundation-acceptance", output.parents)
        self.assertNotEqual(output, self.selection.build_dir)
        self.assertFalse(self.selection.build_dir.exists())
        report = json.loads((self.harness.workspace / "results.json").read_text())
        self.assertEqual(report["input_sha256"], before)
        self.assertEqual(report["windows_and_ci"], "independently pending")
        self.assertEqual(report["phase_verification"], "not performed")

    def test_external_providers_retain_real_selection_and_never_fallback(self):
        self.harness.execute("providers")
        self.assert_no_setup()
        for provider in ("managed", "system", "custom"):
            selection = self.harness.context(provider).selection
            self.assertEqual(selection.provider, provider)
            self.assertEqual(selection.llvm_dir, None if provider == "managed" else self.llvm)
            self.assertEqual(selection.lockfile, self.selection.lockfile)
        installs = [argv for argv, _ in self.calls if argv[1:2] == ["install"]]
        self.assertEqual([argv[argv.index("-o:h") + 1] for argv in installs],
                         ["&:llvm_provider=managed", "&:llvm_provider=system", "&:llvm_provider=custom"])

    def test_missing_external_config_stops_before_any_native_command(self):
        (self.llvm / "LLVMConfig.cmake").unlink()
        with self.assertRaisesRegex(BaselineError, "real explicit.*no fallback"):
            self.harness.execute("providers")
        self.assertEqual(self.calls, [])

    def test_first_cache_or_build_failure_stops_without_retry_setup_or_next_provider(self):
        for failure_point in ("install", "--build"):
            self.calls.clear()
            self.harness.contexts.clear()
            def fail(argv, **kwargs):
                if argv[1] == failure_point:
                    self.calls.append((argv, kwargs))
                    raise subprocess.CalledProcessError(7, argv)
                return self.native_fixture(argv, **kwargs)
            with patch.object(dev, "run_command", side_effect=fail), self.assertRaises(
                    (BaselineError, subprocess.CalledProcessError)):
                self.harness.execute("providers")
            self.assertEqual(sum(argv[1] == failure_point for argv, _ in self.calls), 1)
            self.assertFalse(any(argv[0] == "ctest" for argv, _ in self.calls))
            self.assertEqual(set(self.harness.contexts), {"managed"})
            self.assert_no_setup()

    def test_mismatched_tool_identity_stops_read_only_before_workspace_or_command(self):
        observed = matching_observations(self.baseline, LINUX)
        observed["tools.conan"] = "2.32.0"
        with patch.object(foundation, "collect_observations", return_value=observed), \
             self.assertRaisesRegex(BaselineError, "tools.conan expected=2.33.0; actual=2.32.0"):
            self.harness.execute("prepared")
        self.assertIsNone(self.harness.workspace)
        self.assertEqual(self.calls, [])

    def test_runtime_uses_native_checks_and_strict_existing_ctest_rejection_wrappers(self):
        self.harness.execute("runtime")
        argv = self.calls[-1][0]
        self.assertEqual(argv[0], "ctest")
        self.assertIn("--no-tests=error", argv)
        self.assertIn("WrongVersion|WrongRuntime|WrongTriple", argv[argv.index("-R") + 1])
        self.assert_no_setup()

    def test_all_requires_separately_explicit_setup_evidence_before_any_work(self):
        with self.assertRaisesRegex(BaselineError, "setup remains separate"):
            self.harness.execute("all-checks")
        self.assertEqual(self.calls, [])
        self.assertIsNone(self.harness.workspace)

    def test_cache_snapshot_uses_absolute_cli_and_only_conan_owned_raw_list(self):
        destination = self.root / ".veyr-dev/observations/before.json"
        self.harness.cache_snapshot(destination)
        argv, kwargs = self.calls[0]
        self.assertEqual(argv[:3], [str(self.selection.conan_executable), "list", "*#*:*#*"])
        self.assertIn("--cache", argv)
        self.assertFalse(any(value.startswith("--remote") for value in argv))
        self.assertEqual(kwargs["env"]["CONAN_HOME"], str(self.selection.conan_home))
        self.assertEqual(json.loads(destination.read_text()), cache_fixture())
        self.assert_no_setup()
        with self.assertRaisesRegex(BaselineError, "overwrite"):
            self.harness.cache_snapshot(destination)
        self.assertEqual(len(self.calls), 1)

    def setup_evidence(self, extra=""):
        before = self.root / "build/cache-before.json"
        before.parent.mkdir(exist_ok=True)
        before.write_text(json.dumps(cache_fixture()))
        log = self.root / ".veyr-dev/setup.log"
        argv = ["python", "-B", str(self.root / "scripts/dev.py"), "setup",
                "--profile", self.selection.profile, "--config", self.selection.config,
                "--environment", self.selection.environment, "--conan-home", str(self.selection.conan_home)]
        log.write_text(f"Command: {argv!r}\nWorking directory: {self.root}\n"
                       "llvm/23.1.2@veyr/m1: Already installed!\ngtest/1.18.0: Already installed!\n"
                       "llvm/23.1.2@veyr/m1 (test package): Calling build()\n" + extra + "\nExit: 0; elapsed: 1.0s\n")
        self.harness.cache_before, self.harness.setup_log = before, log
        return before, log

    def test_cache_comparison_requires_matching_successful_setup_and_unchanged_revisions(self):
        self.setup_evidence()
        self.harness.execute("cache")
        self.assertEqual([argv[1] for argv, _ in self.calls], ["list"])
        self.assertIn("cache", self.harness.results)
        self.assert_no_setup()

    def test_dependency_build_download_or_failed_mismatched_setup_is_not_warm_reuse(self):
        for extra in ("llvm/23.1.2@veyr/m1: Calling build()", "Downloading package"):
            self.setup_evidence(extra)
            with self.assertRaisesRegex(BaselineError, "download/build"):
                self.harness.cache()
        _, log = self.setup_evidence()
        log.write_text(log.read_text().replace("Exit: 0", "Exit: 1"))
        with self.assertRaisesRegex(BaselineError, "successful final"):
            self.harness.cache()
        _, log = self.setup_evidence()
        log.write_text(log.read_text().replace("'Debug'", "'Release'"))
        with self.assertRaisesRegex(BaselineError, "differs"):
            self.harness.cache()
        self.assertEqual(self.calls, [])

    def test_changed_package_revision_is_reported_and_no_repair_runs(self):
        self.setup_evidence()
        with patch.object(self.harness, "cache_snapshot", return_value=foundation.package_identities(
                cache_fixture("new-revision"))), self.assertRaisesRegex(BaselineError, "identities changed"):
            self.harness.cache()
        self.assert_no_setup()

    def test_clean_removes_only_disposable_selected_output_preserving_tools_cache_other_outputs(self):
        self.selection.build_dir.mkdir(parents=True)
        (self.selection.build_dir / "normal.txt").write_text("normal output")
        (self.selection.conan_home / "cache-marker").write_text("cached package")
        before = foundation.tree_identity(self.selection.conan_home)
        self.harness.execute("clean")
        self.assertFalse((self.harness.workspace / "clean/Debug").exists())
        self.assertEqual((self.harness.workspace / "clean/Release/preserved.txt").read_text(), "other configuration")
        self.assertEqual((self.selection.build_dir / "normal.txt").read_text(), "normal output")
        self.assertTrue(self.selection.conan_executable.is_file())
        self.assertEqual(foundation.tree_identity(self.selection.conan_home), before)
        self.assertEqual(self.calls, [])

    def test_tree_observation_does_not_follow_symlinks_and_detects_entry_change(self):
        outside = self.directory / "outside"
        outside.mkdir()
        (outside / "not-followed").write_text("external")
        link = self.selection.conan_home / "link"
        link.symlink_to(outside, target_is_directory=True)
        before = foundation.tree_identity(self.selection.conan_home)
        self.assertEqual(before["entries"], 2)
        (outside / "not-followed").write_text("external changed")
        self.assertEqual(foundation.tree_identity(self.selection.conan_home), before)
        (self.selection.conan_home / "new").touch()
        self.assertNotEqual(foundation.tree_identity(self.selection.conan_home), before)

    def test_offline_expected_rejection_is_once_in_empty_external_home_not_real_cache(self):
        homes = []
        def reject(argv, **kwargs):
            self.calls.append((argv, kwargs))
            homes.append(Path(kwargs["env"]["CONAN_HOME"]))
            self.addCleanup(lambda: __import__("shutil").rmtree(homes[-1]))
            self.assertEqual(list(homes[-1].iterdir()), [])
            self.assertIn("--no-remote", argv)
            self.assertIn("--build=never", argv)
            print("gtest/1.18.0: Not found in local cache, looking in remotes...")
            print("Graph root\n    conanfile.py: /fixture/conanfile.py")
            print("ERROR: Package 'gtest/1.18.0' not resolved: No remote defined. Required by ''")
            raise subprocess.CalledProcessError(1, argv)
        before = foundation.tree_identity(self.selection.conan_home)
        with patch.object(foundation, "run_command", side_effect=reject):
            self.harness.execute("offline")
        self.assertEqual(len(self.calls), 1)
        self.assertNotEqual(homes[0], self.selection.conan_home)
        self.assertNotIn(self.root, homes[0].parents)
        self.assertEqual(foundation.tree_identity(self.selection.conan_home), before)
        self.assertEqual(self.harness.results["offline"]["expected_exit"], 1)
        self.setup.assert_not_called()

    def test_offline_unexpected_success_or_nonzero_diagnostic_stops_without_retry(self):
        missing = "gtest/1.18.0: Not found in local cache, looking in remotes..."
        unresolved = "ERROR: Package 'gtest/1.18.0' not resolved: No remote defined. Required by ''"
        expected = f"{missing}\n{unresolved}"
        for exit_code, message in ((0, "ok"), (7, expected), (1, "ERROR: compiler failed"),
                                   (1, missing), (1, unresolved),
                                   (1, expected.replace("gtest/1.18.0", "other/1.18.0")),
                                   (1, f"Downloading package\n{expected}"),
                                   (1, f"Building from source\n{expected}"),
                                   (1, f"Calling build()\n{expected}")):
            self.calls.clear()
            def result(argv, **kwargs):
                self.calls.append((argv, kwargs))
                home = Path(kwargs["env"]["CONAN_HOME"])
                self.addCleanup(lambda path=home: __import__("shutil").rmtree(path))
                print(message)
                if exit_code:
                    raise subprocess.CalledProcessError(exit_code, argv)
                return subprocess.CompletedProcess(argv, 0)
            with patch.object(foundation, "run_command", side_effect=result), self.assertRaises(
                    (BaselineError, subprocess.CalledProcessError)):
                self.harness.offline()
            self.assertEqual(len(self.calls), 1)

    def test_pending_windows_and_ci_do_not_inherit_linux_evidence(self):
        with patch.object(foundation, "read_baseline", return_value=self.baseline), \
             patch.object(foundation.platform, "system", return_value="Linux"), \
             patch.object(foundation.platform, "machine", return_value="x86_64"), \
             patch.dict(os.environ, {"GITHUB_ACTIONS": "true"}), \
             patch.object(foundation, "make_selection") as selection:
            self.assertEqual(foundation.main(["--profile", LINUX, "--config", "Debug",
                                               "--environment", "linux-developer"]), 1)
            selection.assert_not_called()
        self.assertEqual(self.calls, [])


class CIWorkflowPolicyTests(unittest.TestCase):
    def test_authorized_ci_execution_retains_policy_gates(self):
        workflow = (Path(__file__).resolve().parents[2] / ".github/workflows/build-foundation.yml").read_text(encoding="utf-8")
        self.assertIn("VEYR_CI_EXECUTION_AUTHORIZED: 'true'", workflow)
        gate = workflow.index("if ($env:VEYR_CI_EXECUTION_AUTHORIZED -ne 'true')")
        self.assertLess(gate, workflow.index("- name: Select exact reviewed base Python"))
        self.assertLess(gate, workflow.index("- name: Bootstrap project-owned Conan CLI only"))
        self.assertLess(gate, workflow.index("- name: Explicit Conan dependency setup"))
        self.assertIn("requires separate owner authorization", workflow)
        lock_gate = workflow.index("Required tracked native Conan lock missing")
        self.assertLess(lock_gate, workflow.index("- name: Restore portable Conan package archive only"))
        self.assertLess(lock_gate, workflow.index("- name: Explicit Conan dependency setup"))
        self.assertIn("if ($entry.status -notin @('selected', 'reviewed'))", workflow)

    def test_windows_compiler_selection_follows_devshell_and_uses_absolute_standalone_paths(self):
        workflow = (Path(__file__).resolve().parents[2] / ".github/workflows/build-foundation.yml").read_text(encoding="utf-8")
        setup = workflow[workflow.index("- name: Set up conventional isolated CMake/Ninja prerequisites"):
                         workflow.index("- name: Bootstrap project-owned Conan CLI only")]
        self.assertLess(setup.index("Enter-VsDevShell"), setup.index("$standaloneBin ="))
        self.assertIn("$standaloneBin = 'C:\\Program Files\\LLVM\\bin'", setup)
        self.assertIn("$banner = & $executable '--version'", setup)
        self.assertIn('$env:PATH = "$standaloneBin;$env:PATH"', setup)
        self.assertIn("no bundled LLVM fallback", setup)


class WindowsDevShellProfileTests(unittest.TestCase):
    """Protect externally validated DevShell reuse without launching Conan."""

    def setUp(self):
        self.root = Path(__file__).resolve().parents[2]
        self.text = (self.root / "conan/profiles/windows-x86_64-clangcl").read_text(encoding="utf-8")
        self.profile = configparser.ConfigParser(interpolation=None, delimiters=("=",))
        self.profile.optionxform = str
        self.profile.read_string(self.text)

    def test_vcvars_activation_is_disabled_with_an_unquoted_empty_value(self):
        key = "tools.microsoft.msbuild:installation_path"
        self.assertIn(key, self.profile["conf"])
        self.assertEqual(self.profile["conf"][key], "")
        self.assertEqual(re.findall(r"^" + re.escape(key) + r"=(.*)$",
                                    self.text, re.MULTILINE), [""])

    def test_reviewed_windows_compiler_and_runtime_settings_remain_intact(self):
        self.assertEqual(dict(self.profile["settings"]), {
            "os": "Windows", "arch": "x86_64", "compiler": "clang",
            "compiler.version": "20", "compiler.cppstd": "23",
            "compiler.runtime": "dynamic", "compiler.runtime_version": "v143",
            "llvm/*:build_type": "Release",
        })
        self.assertEqual(self.profile["conf"]["tools.build:compiler_executables"],
                         '{{ {"c": os.environ["VEYR_C_COMPILER"], "cpp": os.environ["VEYR_CPP_COMPILER"]} | tojson }}')
        self.assertEqual(self.profile["conf"]["tools.cmake.cmaketoolchain:extra_variables"],
                         '{{ {"CMAKE_LINKER": os.environ["VEYR_LINKER"]} | tojson }}')

    def test_devshell_vc_sdk_environment_remains_explicit(self):
        expected = {
            "VSINSTALLDIR": "VEYR_VS_ROOT", "VCToolsInstallDir": "VEYR_VCTOOLS_ROOT",
            "VCToolsVersion": "VEYR_VCTOOLS_VERSION", "WindowsSdkDir": "VEYR_WINDOWS_SDK_DIR",
            "WindowsSDKVersion": "VEYR_WINDOWS_SDK_VERSION", "UCRTVersion": "VEYR_WINDOWS_SDK_VERSION",
        }
        self.assertEqual(set(self.profile["buildenv"]), set(expected))
        for key, variable in expected.items():
            with self.subTest(key=key):
                value = '{{ os.environ["' + variable + '"] }}'
                if key == "WindowsSDKVersion":
                    value += "\\"
                self.assertEqual(self.profile["buildenv"][key], value)

    def test_runtime_overlays_preserve_debug_host_and_release_build_context(self):
        debug = (self.root / "conan/profiles/debug").read_text(encoding="utf-8")
        release = (self.root / "conan/profiles/release").read_text(encoding="utf-8")
        self.assertIn('build_type={{ "Release" if context == "build" else "Debug" }}', debug)
        self.assertIn('compiler.runtime_type={{ "Release" if context == "build" else "Debug" }}', debug)
        self.assertIn('build_type=Release', release)
        self.assertIn('compiler.runtime_type=Release', release)
        for overlay in (debug, release):
            self.assertIn('{% if os.environ["VEYR_PROFILE_OS"] == "Windows" %}', overlay)
            self.assertNotIn("tools.microsoft.msbuild:installation_path", overlay)

    def test_linux_profile_keeps_its_separate_compiler_and_gcc_policy(self):
        linux = (self.root / "conan/profiles/linux-x86_64-clang").read_text(encoding="utf-8")
        self.assertIn("os=Linux", linux)
        self.assertIn('compiler.version={{ os.environ["VEYR_CLANG_MAJOR"] }}', linux)
        self.assertIn("compiler.libcxx=libstdc++11", linux)
        self.assertIn('os.environ["VEYR_GCC_INSTALL_DIR"]', linux)
        self.assertNotIn("tools.microsoft.msbuild:installation_path", linux)
        self.assertNotIn("compiler.runtime", linux)
        self.assertNotIn("[buildenv]", linux)


class ConanCacheWorkflowTests(unittest.TestCase):
    """Evaluate actual workflow guards and key code without launching any CLI."""

    def setUp(self):
        self.root = Path(__file__).resolve().parents[2]
        self.workflow = (self.root / ".github/workflows/build-foundation.yml").read_text(encoding="utf-8")
        self.blocks = dict(re.findall(
            r"^      - name: ([^\n]+)\n(.*?)(?=^      - name: |\Z)",
            self.workflow, re.MULTILINE | re.DOTALL))
        self.names = list(self.blocks)
        self.keys = "Compute Conan cache compatibility and snapshot keys"
        self.restore = "Restore portable Conan package archive only (optimization)"
        self.import_archive = "Import restored archive into the isolated Conan home"
        self.setup = "Explicit Conan dependency setup (downloads/source builds allowed)"
        self.export = "Check dependency integrity and export portable Conan archive"
        self.save = "Save completed Conan package snapshot before project validation"
        self.baseline = json.loads((self.root / "scripts/devtools/baseline.json").read_text())

    def enabled(self, name, *, cold=False, success=True, outcomes=None, matched="snapshot"):
        guard = re.search(r"^\s+if: \$\{\{ (.*?) \}\}$", self.blocks[name], re.MULTILINE)
        if guard is None:
            return success  # GitHub's default step guard.
        expression = guard.group(1).replace("success()", str(success))
        expression = expression.replace("inputs.cold_build", str(cold))
        expression = expression.replace("steps.cache_restore.outputs.cache-matched-key", repr(matched))
        expression = re.sub(r"steps\.([a-z_]+)\.outcome",
                            lambda match: repr((outcomes or {}).get(match.group(1), "skipped")),
                            expression)
        expression = expression.replace("&&", " and ")
        expression = re.sub(r"!(?!=)", " not ", expression)
        return eval(expression.strip(), {"__builtins__": {}}, {})

    def test_restore_import_setup_export_save_order_and_windows_lock_gate(self):
        order = ("Strict read-only prerequisite doctor", self.keys, self.restore,
                 self.import_archive, self.setup, self.export, self.save,
                 "Native cache-only configure", "Native incremental build", "Native test",
                 "Native verify", "Foundation prepared native acceptance",
                 "Foundation acquisition-disabled empty-cache rejection")
        self.assertEqual(sorted(order, key=self.names.index), list(order))
        self.assertIn("Required tracked native Conan lock missing",
                      self.blocks["Strict read-only prerequisite doctor"])
        self.assertIn("if ($code -ne 0) { exit $code }", self.blocks[self.setup])
        for name in (self.import_archive, self.export):
            block = self.blocks[name]
            self.assertIn("VEYR_CONAN_EXECUTABLE: ${{ steps.doctor.outputs.conan }}", block)
            self.assertIn("& $env:VEYR_CONAN_EXECUTABLE cache ", block)
            self.assertIn("if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }", block)
        export = self.blocks[self.export]
        self.assertLess(export.index("cache check-integrity"), export.index("exit $LASTEXITCODE"))
        self.assertLess(export.index("exit $LASTEXITCODE"), export.index("cache save"))
        self.assertEqual(export.count("exit $LASTEXITCODE"), 2)
        self.assertIn("cache save '*#*:*#*'", export)
        self.assertNotIn("package_id", export)  # Conan retains the distinct IDs/revisions.
        self.assertNotIn("if:", self.blocks[self.setup])
        self.assertIn("if (-not (Test-Path -LiteralPath $archive -PathType Leaf))",
                      self.blocks[self.import_archive])

    def test_archive_only_pinned_actions_and_complete_restore_prefix(self):
        pin = "5a3ec84eff668545956fd18022155c47e93e2684"
        for name, action in ((self.restore, "restore"), (self.save, "save")):
            block = self.blocks[name]
            self.assertIn(f"actions/cache/{action}@{pin}", block)
            self.assertIn("path: .veyr-dev/conan-cache/dependencies.tgz", block)
            self.assertIn("key: ${{ steps.cache_keys.outputs.key }}", block)
            self.assertIn("enableCrossOsArchive: false", block)
            self.assertNotIn("CONAN_HOME", block)
        self.assertIn("restore-keys: ${{ steps.cache_keys.outputs.prefix }}",
                      self.blocks[self.restore])
        self.assertEqual(self.workflow.count("restore-keys:"), 1)
        self.assertNotIn("uses: actions/cache@", self.workflow)
        self.assertIn("veyr-conan-${{ github.run_id }}-${{ github.run_attempt }}-${{ matrix.environment }}-${{ matrix.config }}",
                      self.workflow)

    def test_setup_or_integrity_export_failure_never_saves(self):
        for dependencies, export in (("failure", "skipped"), ("skipped", "skipped"),
                                     ("success", "failure"), ("success", "skipped")):
            outcomes = {"dependencies": dependencies, "export_cache": export}
            with self.subTest(outcomes=outcomes):
                self.assertFalse(self.enabled(self.save, outcomes=outcomes))
        self.assertFalse(self.enabled(self.export, outcomes={"dependencies": "failure"}))
        self.assertTrue(self.enabled(self.save, outcomes={
            "dependencies": "success", "export_cache": "success"}))

    def test_later_project_failures_happen_after_successful_snapshot_save(self):
        for failure in ("Native cache-only configure", "Native incremental build",
                        "Native test", "Native verify"):
            outcomes, saved, success = {}, False, True
            for name in self.names:
                if name == self.save and self.enabled(name, outcomes=outcomes, success=success):
                    saved = True
                if name == self.setup:
                    outcomes["dependencies"] = "success"
                if name == self.export:
                    outcomes["export_cache"] = "success"
                if name == failure:
                    self.assertTrue(saved, failure)
                    success = False
            self.assertTrue(saved)  # No final-job-success post action controls this save.

    def test_cold_build_bypasses_all_cache_transfer_but_not_explicit_setup(self):
        outcomes = {"dependencies": "success", "export_cache": "success"}
        for name in (self.keys, self.restore, self.import_archive, self.export, self.save):
            with self.subTest(name=name):
                self.assertFalse(self.enabled(name, cold=True, outcomes=outcomes))
                self.assertFalse(self.enabled(name, success=False, outcomes=outcomes))
        self.assertTrue(self.enabled(self.setup, cold=True))
        self.assertFalse(self.enabled(self.import_archive, matched=""))
        self.assertTrue(self.enabled(self.import_archive, matched="compatible-prefix-snapshot"))

    def compute_keys(self, baseline=None, config="Debug", **overrides):
        script = re.search(r"\$keyScript = @'\n(.*?)\n\s*'@",
                           self.blocks[self.keys], re.DOTALL).group(1)
        script = textwrap.dedent(script)
        env = {"VEYR_CI_ENVIRONMENT": "ubuntu-ci", "VEYR_CI_PROFILE": LINUX,
               "VEYR_CI_CONFIG": config, "RUNNER_OS": "Linux", "RUNNER_ARCH": "X64",
               "VEYR_CACHE_IMAGE": "image-1", "VEYR_DEPENDENCY_INPUT_HASH": "a" * 64,
               "GITHUB_RUN_ID": "123", "GITHUB_RUN_ATTEMPT": "1", **overrides}
        with tempfile.TemporaryDirectory() as directory:
            output = Path(directory) / "github-output"
            env["GITHUB_OUTPUT"] = str(output)
            with patch.dict(os.environ, env, clear=True), \
                 patch.object(Path, "read_text", return_value=json.dumps(baseline or self.baseline)), \
                 contextlib.redirect_stdout(io.StringIO()):
                exec(compile(script, "<workflow-cache-keys>", "exec"), {})
            return dict(line.split("=", 1) for line in output.read_text().splitlines())

    def test_debug_release_share_compatibility_but_have_distinct_snapshot_keys(self):
        debug, release = self.compute_keys(), self.compute_keys(config="Release")
        self.assertEqual(debug["prefix"], release["prefix"])
        self.assertNotEqual(debug["key"], release["key"])
        self.assertEqual(debug["key"], debug["prefix"] + "123-1-Debug")
        self.assertEqual(release["key"], release["prefix"] + "123-1-Release")
        self.assertNotEqual(debug["key"], self.compute_keys(GITHUB_RUN_ATTEMPT="2")["key"])
        self.assertNotEqual(debug["key"], self.compute_keys(GITHUB_RUN_ID="124")["key"])
        self.assertIn("conan-v2-Linux-X64-ubuntu-ci-linux-x86_64-clang-image-1-", debug["prefix"])
        windows = {"VEYR_CI_ENVIRONMENT": "windows-ci",
                   "VEYR_CI_PROFILE": "windows-x86_64-clangcl", "RUNNER_OS": "Windows"}
        debug, release = self.compute_keys(**windows), self.compute_keys(config="Release", **windows)
        self.assertEqual(debug["prefix"], release["prefix"])
        self.assertNotEqual(debug["key"], release["key"])

    def test_evidence_git_status_and_unrelated_baselines_do_not_change_prefix(self):
        changed = copy.deepcopy(self.baseline)
        changed["platforms"] = {"irrelevant": {"tools": {"clang": "99.0.0"}}}
        changed["ci_environments"]["windows-ci"]["native"]["sdk"] = "unrelated"
        entry = changed["ci_environments"]["ubuntu-ci"]
        entry.update(status="selected", evidence="new run", remaining_gaps=[], prior_observations=[])
        entry["observed_candidate"] = {"git": "new utility observation"}
        entry["tools"]["git"] = "99.0.0"
        changed["tools"]["git"] = "98.0.0"
        self.assertEqual(self.compute_keys()["prefix"], self.compute_keys(changed)["prefix"])

    def test_critical_toolchain_image_or_dependency_changes_isolate_snapshots(self):
        before = self.compute_keys()["prefix"]
        for section, name in (("tools", "clang"), ("native", "libc"), ("paths", "gcc_install_dir")):
            changed = copy.deepcopy(self.baseline)
            changed["ci_environments"]["ubuntu-ci"][section][name] = "changed"
            self.assertNotEqual(before, self.compute_keys(changed)["prefix"])
        for env in ({"VEYR_CACHE_IMAGE": "image-2"}, {"VEYR_DEPENDENCY_INPUT_HASH": "b" * 64},
                    {"RUNNER_ARCH": "different"}):
            self.assertNotEqual(before, self.compute_keys(**env)["prefix"])
        for section, field in (("tools", "conan"), ("tools", "ninja"), ("backend", "llvm")):
            changed = copy.deepcopy(self.baseline)
            changed[section][field] = "changed"
            self.assertNotEqual(before, self.compute_keys(changed)["prefix"])
        windows = self.compute_keys(VEYR_CI_ENVIRONMENT="windows-ci",
                                    VEYR_CI_PROFILE="windows-x86_64-clangcl", RUNNER_OS="Windows")
        self.assertNotEqual(before, windows["prefix"])
        with self.assertRaisesRegex(SystemExit, "Missing Conan dependency-input hash"):
            self.compute_keys(VEYR_DEPENDENCY_INPUT_HASH="")

    def test_dependency_hash_covers_only_selected_package_inputs(self):
        expression = re.search(r"VEYR_DEPENDENCY_INPUT_HASH: (\$\{\{.*?\}\})",
                               self.blocks[self.keys]).group(1)
        for value in ("format('conan/profiles/{0}', matrix.profile)", "conan/profiles/debug",
                      "conan/profiles/release", "format('conan/locks/{0}-*.lock', matrix.profile)",
                      "conan/recipes/llvm/**", "conanfile.py"):
            self.assertIn(value, expression)
        for value in ("baseline.json", "CMakePresets.json", "matrix.config", "conan/profiles/**",
                      "conan/locks/**"):
            self.assertNotIn(value, expression)


if __name__ == "__main__":
    unittest.main()
