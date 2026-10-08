"""Temporary-tree command boundaries; no Conan/native build commands."""

import contextlib
from dataclasses import replace
import io
import json
import os
from pathlib import Path
import subprocess
import sys
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "scripts"))
import dev
from devtools import conan_ops, doctor
from devtools.doctor import BaselineError, make_selection, select_inputs
from test_conan_ops import ConanOperationsFixture
from test_doctor import LINUX, WINDOWS


class DeveloperCommandsTests(ConanOperationsFixture, unittest.TestCase):
    def setUp(self):
        super().setUp()
        (self.root / "CMakeLists.txt").touch()
        (self.root / "CMakePresets.json").touch()
        self.context = dev.Operations(self.selection, self.selected, root=self.root)
        self.output = io.StringIO()
        self.addCleanup(patch.stopall)
        patch("sys.stdout", self.output).start()
        patch.object(dev, "run_command", side_effect=self.fake_run).start()
        self.setup = patch.object(dev, "setup_dependencies", side_effect=self.fake_setup).start()
        self.bootstrap = patch.object(dev, "bootstrap_conan").start()

    def fake_setup(self, *args, **kwargs):
        kwargs["ensure_prerequisites"]()
        self.calls.append((["setup"], kwargs))

    def configure_outputs(self):
        directory = self.context.selection.build_dir
        directory.mkdir(parents=True, exist_ok=True)
        for name in ("CMakeCache.txt", "build.ninja", "CTestTestfile.cmake"):
            (directory / name).write_text("fixture\n")
        (directory / "tests").mkdir(exist_ok=True)
        (directory / "tests/CTestTestfile.cmake").write_text('add_test("include-policy" "python")\n')
        compiler = self.context.selected["environment"]["VEYR_CPP_COMPILER"]
        if doctor.PROFILES[self.context.selection.profile] == "Windows":
            compiler = f'"{compiler}"'
        (directory / "compile_commands.json").write_text(json.dumps([{
            "directory": str(directory), "file": str(self.root / "fixture.cpp"),
            "command": compiler + " -std=c++23 -c fixture.cpp"}]))

    def fake_run(self, argv, **kwargs):
        self.calls.append((argv, kwargs))
        if argv[:2] == ["cmake", "--preset"]:
            self.configure_outputs()
        if argv[:2] == ["cmake", "--build"]:
            suffix = ".exe" if doctor.PROFILES[self.context.selection.profile] == "Windows" else ""
            for name in ("veyrc", "tests/foundation_tests", "tests/llvm_link_tests"):
                (self.context.selection.build_dir / (name + suffix)).touch()
        return subprocess.CompletedProcess(argv, 0)

    def assert_cache_only(self):
        for argv, kwargs in self.calls:
            self.assertEqual(kwargs["cwd"], self.root)
            if argv[0] == str(self.context.selection.conan_executable):
                self.assertEqual(argv[1], "install")
                self.assertIn("--no-remote", argv)
                self.assertIn("--build=never", argv)
                self.assertNotIn("--build=missing", argv)
                self.assertEqual(kwargs["env"]["CONAN_HOME"], str(self.context.selection.conan_home))
        self.setup.assert_not_called()
        self.bootstrap.assert_not_called()

    def test_build_configures_cache_only_then_builds_once(self):
        self.context.build()
        self.context.build()
        self.assertEqual([argv[1] for argv, _ in self.calls], ["install", "--preset", "--build"])
        self.assert_cache_only()

    def test_prepared_build_skips_configure_and_test_builds_once(self):
        self.context.configure()
        self.calls.clear()
        context = dev.Operations(self.selection, self.selected, root=self.root)
        context.test()
        context.test()
        self.assertEqual([argv[0] for argv, _ in self.calls], ["cmake", "ctest"])
        self.assertIn("--no-tests=error", self.calls[-1][0])
        self.assert_cache_only()

    def test_all_is_setup_configure_build_ctest_verify_without_duplicate_work(self):
        self.context.all()
        self.context.all()
        self.assertEqual([argv[0] for argv, _ in self.calls],
                         ["setup", str(self.selection.conan_executable), "cmake", "cmake", "ctest"])
        self.setup.assert_called_once()
        self.assertEqual(self.context.done, {"prerequisites", "setup", "configure", "build", "test", "ctest", "verify"})
        self.assertIn("Reusing equivalent CTest", self.output.getvalue())

    def test_verify_has_no_build_and_requires_prepared_native_outputs(self):
        self.context.configure()
        self.calls.clear()
        with self.assertRaisesRegex(BaselineError, "explicit build"):
            self.context.verify()
        self.assertEqual(self.calls, [])
        self.context.build()
        self.calls.clear()
        self.context.verify()
        self.assertEqual([argv[0] for argv, _ in self.calls], ["ctest"])
        self.assert_cache_only()

    def test_verify_requires_include_registration_and_valid_compile_database(self):
        self.context.build()
        registration = self.selection.build_dir / "tests/CTestTestfile.cmake"
        registration.write_text("other tests")
        self.calls.clear()
        with self.assertRaisesRegex(BaselineError, "include-policy"):
            self.context.verify()
        self.assertEqual(self.calls, [])
        registration.write_text('add_test("include-policy" "python")')
        for payload in ([], [{"file": "x", "directory": str(self.selection.build_dir),
                               "command": "wrong-compiler -std=c++20"}]):
            (self.selection.build_dir / "compile_commands.json").write_text(json.dumps(payload))
            with self.assertRaisesRegex(BaselineError, "Invalid compilation database"):
                self.context.verify()
        self.assertEqual(self.calls, [])

    def test_verify_accepts_exact_include_policy_serialization_forms(self):
        self.context.build()
        registration = self.selection.build_dir / "tests/CTestTestfile.cmake"
        for command in (
                'add_test("include-policy" "python")',
                'add_test([=[include-policy]=] "python")',
                'add_test([[include-policy]] "python")',
                'add_test([==[include-policy]==] "python")',
                'add_test([=[\ninclude-policy]=] "python")',
                'add_test(include-policy "python")',
                '  ADD_TEST (\n "include-policy"\n "python")'):
            with self.subTest(command=command):
                registration.write_text(command)
                self.calls.clear()
                context = dev.Operations(self.selection, self.selected, root=self.root)
                context.done.add("configure")  # Prepared fixture; only verify may run.
                context.verify()
                self.assertEqual([argv[0] for argv, _ in self.calls], ["ctest"])
                self.assert_cache_only()

    def test_verify_rejects_incorrect_include_policy_names_and_mentions(self):
        self.context.build()
        registration = self.selection.build_dir / "tests/CTestTestfile.cmake"
        for command in (
                'add_test("include-policy-extra" "python")',
                'add_test([=[include-policy-extra]=] "python")',
                'add_test(include-policy-extra "python")',
                'add_test("other-include-policy" "python")',
                'add_test("Include-policy" "python")',
                'add_test("other" "include-policy")',
                'set_tests_properties("include-policy" PROPERTIES LABELS "policy")',
                '# add_test("include-policy" "python")',
                '#[==[\nadd_test([=[include-policy]=] "python")\n]==]',
                'set(example "\nadd_test(\\"include-policy\\" \\"python\\")\n")',
                'set(example [==[\nadd_test([=[include-policy]=] "python")\n]==])',
                'add_test([=[include-policy]==] "python")',
                'add_test("other" "python")',
                ''):
            with self.subTest(command=command):
                registration.write_text(command)
                self.calls.clear()
                context = dev.Operations(self.selection, self.selected, root=self.root)
                context.done.add("configure")
                with self.assertRaisesRegex(BaselineError, "Missing include-policy CTest registration"):
                    context.verify()
                self.assertEqual(self.calls, [])
                self.assert_cache_only()

    def test_verify_rejects_missing_include_policy_registration_file(self):
        self.context.build()
        (self.selection.build_dir / "tests/CTestTestfile.cmake").unlink()
        self.calls.clear()
        with self.assertRaisesRegex(BaselineError, "Missing include-policy CTest registration"):
            self.context.verify()
        self.assertEqual(self.calls, [])
        self.assert_cache_only()

    def test_stale_profile_lock_tool_or_generated_identity_regenerates_cache_only(self):
        for path in (self.selected["host_profiles"][0], self.selection.lockfile,
                     self.root / "scripts/devtools/baseline.json",
                     self.selection.build_dir / "generators/conan_toolchain.cmake"):
            self.context.configure()
            path.parent.mkdir(parents=True, exist_ok=True)
            path.write_text("changed identity")
            self.calls.clear()
            context = dev.Operations(self.selection, self.selected, root=self.root)
            context.build()
            self.assertEqual([argv[1] for argv, _ in self.calls], ["install", "--preset", "--build"])
            self.assert_cache_only()
            self.context = context
            self.context.done.clear()

    def test_missing_prepared_lock_or_home_stops_before_child(self):
        for selection in (replace(self.selection, lockfile=self.selection.lockfile.with_name("missing.lock")),
                          replace(self.selection, conan_home=self.directory / "absent cache")):
            context = dev.Operations(selection, self.selected, root=self.root)
            with self.assertRaisesRegex(BaselineError, "explicit setup"):
                context.build()
        self.assertEqual(self.calls, [])
        self.assert_cache_only()

    def test_cache_miss_stops_once_without_setup_build_retry(self):
        def fail(argv, **kwargs):
            self.calls.append((argv, kwargs))
            raise subprocess.CalledProcessError(7, argv)
        with patch.object(dev, "run_command", side_effect=fail), self.assertRaisesRegex(BaselineError, "exit=7"):
            self.context.test()
        self.assertEqual(len(self.calls), 1)
        self.assert_cache_only()

    def test_build_failure_does_not_run_test_or_verify(self):
        self.context.configure()
        self.calls.clear()
        with patch.object(dev, "run_command", side_effect=subprocess.CalledProcessError(9, ["cmake"])), \
             self.assertRaises(subprocess.CalledProcessError):
            self.context.test()
        self.assertNotIn("build", self.context.done)
        self.assertNotIn("ctest", self.context.done)

    def test_external_provider_keeps_selection_and_cache_only_policy(self):
        llvm = self.directory / "LLVM with spaces"
        llvm.mkdir()
        (llvm / "LLVMConfig.cmake").touch()
        for provider in ("custom", "system"):
            self.calls.clear()
            self.context = dev.Operations(replace(self.selection, provider=provider, llvm_dir=llvm,
                                                build_dir=self.root / "build" / provider / "Debug"),
                                          self.selected, root=self.root)
            self.context.build()
            self.assertIn(f"&:llvm_provider={provider}", self.calls[0][0])
            self.assertIn(f"-DVEYR_LLVM_DIR={llvm}", self.calls[1][0])
            self.assert_cache_only()

    def windows_verify_fixture(self, configuration="Debug"):
        selected = select_inputs(self.baseline, WINDOWS, configuration, root=self.root)
        selected["environment"]["VEYR_CPP_COMPILER"] = r"C:\Program Files\LLVM\bin\clang-cl.exe"
        with patch.object(doctor, "_probe", return_value=str(self.root / ".git")):
            selection = make_selection(self.baseline, selected, root=self.root,
                                       conan_home=self.directory / "Windows Conan cache")
        selection.conan_home.mkdir()
        selection.conan_executable.parent.mkdir(parents=True)
        selection.conan_executable.touch()
        selection.lockfile.touch()
        for path in (*selected["host_profiles"], *selected["build_profiles"]):
            path.touch()
        self.context = dev.Operations(selection, selected, root=self.root)
        self.context.build()
        self.calls.clear()
        database = selection.build_dir / "compile_commands.json"
        entry = json.loads(database.read_text())[0]
        return database, entry, selected["environment"]["VEYR_CPP_COMPILER"]

    def test_windows_release_fixture_paths_and_executable_suffix(self):
        database, entry, compiler = self.windows_verify_fixture("Release")
        self.context.test()
        entry["arguments"] = [compiler,
                              "/std:c++latest", "/c", "fixture.cpp"]
        entry.pop("command")
        database.write_text(json.dumps([entry]))
        with patch.object(dev.os.path, "samefile", return_value=True) as samefile:
            self.context.verify()
        samefile.assert_called_once_with(compiler, compiler)
        self.assertTrue((self.context.selection.build_dir / "veyrc.exe").is_file())
        self.assertEqual(len([a for a, _ in self.calls if a[0] == "ctest"]), 1)
        self.assert_cache_only()

    def test_windows_verify_accepts_native_short_long_compiler_alias_and_forwarded_cpp23(self):
        database, entry, compiler = self.windows_verify_fixture()
        short = r"C:\PROGRA~1\LLVM\bin\clang-cl.exe"
        entry["command"] = short + " /nologo -TP -clang:-std=c++23 -MDd /c fixture.cpp"
        database.write_text(json.dumps([entry]))
        with patch.object(dev.os.path, "samefile", return_value=True) as samefile:
            self.context.verify()
        samefile.assert_called_once_with(short, compiler)
        self.assertEqual([argv[0] for argv, _ in self.calls], ["ctest"])
        self.assert_cache_only()

    def test_windows_verify_accepts_existing_and_forwarded_standard_tokens(self):
        database, entry, compiler = self.windows_verify_fixture()
        for representation in ("command", "arguments"):
            for standard in ("-std=c++23", "/std:c++23", "/std:c++latest", "-clang:-std=c++23"):
                with self.subTest(representation=representation, standard=standard):
                    payload = {k: v for k, v in entry.items() if k not in {"command", "arguments"}}
                    payload[representation] = (f'"{compiler}" {standard} /c fixture.cpp'
                                               if representation == "command" else
                                               [compiler, standard, "/c", "fixture.cpp"])
                    database.write_text(json.dumps([payload]))
                    self.context.done.difference_update({"verify", "ctest"})
                    self.calls.clear()
                    with patch.object(dev.os.path, "samefile", return_value=True) as samefile:
                        self.context.verify()
                    samefile.assert_called_once_with(compiler, compiler)
                    self.assertEqual([argv[0] for argv, _ in self.calls], ["ctest"])
        self.assert_cache_only()

    def test_windows_verify_rejects_different_compiler_even_with_same_basename(self):
        database, entry, compiler = self.windows_verify_fixture()
        different = r"C:\Different LLVM\bin\clang-cl.exe"
        # Mentioning the reviewed compiler as another argument must not satisfy identity.
        entry["command"] = f'"{different}" -clang:-std=c++23 "{compiler}" /c fixture.cpp'
        database.write_text(json.dumps([entry]))
        with patch.object(dev.os.path, "samefile", return_value=False) as samefile:
            with self.assertRaisesRegex(BaselineError, "Invalid compilation database"):
                self.context.verify()
        samefile.assert_called_once_with(different, compiler)
        self.assertEqual(self.calls, [])

    def test_windows_verify_rejects_unresolvable_or_malformed_compiler_identity(self):
        database, entry, compiler = self.windows_verify_fixture()
        with patch.object(dev.os.path, "samefile", side_effect=FileNotFoundError("compiler missing")):
            database.write_text(json.dumps([entry]))
            with self.assertRaisesRegex(BaselineError, "Invalid compilation database"):
                self.context.verify()
        for command in ('"C:\\Program Files\\LLVM\\bin\\clang-cl.exe -clang:-std=c++23',
                        'clang-cl.exe -clang:-std=c++23',
                        f'"{compiler}"extra -clang:-std=c++23'):
            with self.subTest(command=command):
                entry["command"] = command
                database.write_text(json.dumps([entry]))
                with patch.object(dev.os.path, "samefile") as samefile:
                    with self.assertRaisesRegex(BaselineError, "Invalid compilation database"):
                        self.context.verify()
                samefile.assert_not_called()
        self.assertEqual(self.calls, [])

    def test_windows_verify_rejects_wrong_forwarded_standard_or_unrelated_argument(self):
        database, entry, compiler = self.windows_verify_fixture()
        for standard in ("-clang:-std=c++20", "-clang:-std=c++26", "-clang:-Wall",
                         "-clang:-std=c++23-extra", "-Dexample=-clang:-std=c++23"):
            with self.subTest(standard=standard):
                entry["command"] = f'"{compiler}" {standard} /c fixture.cpp'
                database.write_text(json.dumps([entry]))
                with patch.object(dev.os.path, "samefile", return_value=True):
                    with self.assertRaisesRegex(BaselineError, "Invalid compilation database"):
                        self.context.verify()
        self.assertEqual(self.calls, [])

    def test_windows_verify_rejects_wrong_selected_build_directory(self):
        database, entry, compiler = self.windows_verify_fixture()
        entry["directory"] = str(self.context.selection.build_dir.with_name("Release"))
        entry["command"] = f'"{compiler}" -clang:-std=c++23 /c fixture.cpp'
        database.write_text(json.dumps([entry]))
        with patch.object(dev.os.path, "samefile", return_value=True):
            with self.assertRaisesRegex(BaselineError, "Invalid compilation database"):
                self.context.verify()
        self.assertEqual(self.calls, [])

    def test_linux_verify_keeps_cpp23_matching_without_windows_file_identity(self):
        self.context.build()
        self.calls.clear()
        with patch.object(dev.os.path, "samefile") as samefile:
            self.context.verify()
        samefile.assert_not_called()
        self.assertEqual([argv[0] for argv, _ in self.calls], ["ctest"])
        self.assert_cache_only()

    def test_clean_preserves_other_config_tool_environment_and_cache(self):
        self.context.build()
        other = self.selection.build_dir.with_name("Release")
        other.mkdir()
        (other / "keep").write_text("other output")
        (self.selection.conan_home / "keep").write_text("package")
        tool_bytes = self.selection.conan_executable.read_bytes()
        self.calls.clear()
        self.context.clean()
        self.assertFalse(self.selection.build_dir.exists())
        self.assertEqual((other / "keep").read_text(), "other output")
        self.assertEqual((self.selection.conan_home / "keep").read_text(), "package")
        self.assertEqual(self.selection.conan_executable.read_bytes(), tool_bytes)
        self.assertEqual(self.calls, [])
        self.context.clean()  # Missing selected output is harmless.

    def test_clean_rejects_roots_escapes_cache_tool_and_redirected_paths(self):
        for path in (self.root, self.root / "build", self.root / "build/../..",
                     self.selection.conan_home, self.selection.conan_executable.parent.parent):
            with self.subTest(path=path), self.assertRaises(BaselineError):
                dev.Operations(replace(self.selection, build_dir=path), self.selected, root=self.root).clean()
        link = self.root / "build/redirect"
        link.parent.mkdir(exist_ok=True)
        link.symlink_to(self.selection.conan_home, target_is_directory=True)
        with self.assertRaisesRegex(BaselineError, "redirected"):
            dev.Operations(replace(self.selection, build_dir=link / "Debug"), self.selected, root=self.root).clean()
        with patch.object(Path, "is_junction", return_value=True), self.assertRaisesRegex(BaselineError, "redirected"):
            self.context.clean()
        self.assertTrue(self.selection.conan_executable.exists())
        self.assertTrue(self.selection.conan_home.exists())

    def test_supervised_setup_handoff_preserves_absolute_cli_and_package_policy(self):
        conan_ops.setup_dependencies(self.selection, self.selected, root=self.root,
                                     runner=dev.run_command)
        self.assertEqual([argv[1] for argv, _ in self.calls], ["export", "install", "test"])
        self.assertIn("--build=missing", self.calls[1][0])
        self.assertIn("--no-remote", self.calls[2][0])
        self.assertIn("--build=never", self.calls[2][0])
        for argv, kwargs in self.calls:
            self.assertEqual(argv[0], str(self.selection.conan_executable))
            self.assertEqual(kwargs["env"]["CONAN_HOME"], str(self.selection.conan_home))

    def test_cli_defaults_and_explicit_configuration_provider_home_overrides(self):
        for args, configuration, provider in ((["build"], "Debug", "managed"),
                (["test", "--config", "Release", "--llvm-provider", "custom",
                  "--llvm-dir", str(self.directory / "External LLVM"),
                  "--conan-home", str(self.directory / "Explicit cache")], "Release", "custom")):
            with patch.object(dev, "read_baseline", return_value=self.baseline), \
                 patch.object(dev.platform, "system", return_value="Linux"), \
                 patch.object(dev.platform, "machine", return_value="x86_64"), \
                 patch.dict(os.environ, {}, clear=True), \
                 patch.object(dev, "make_selection", return_value=self.selection) as choose, \
                 patch.object(dev, "Operations") as operations:
                self.assertEqual(dev.main(args), 0)
            self.assertEqual(choose.call_args.args[1]["profile"], LINUX)
            self.assertEqual(choose.call_args.args[1]["configuration"], configuration)
            self.assertEqual(choose.call_args.args[2], provider)
            if provider == "custom":
                self.assertEqual(choose.call_args.args[3], self.directory / "External LLVM")
                self.assertEqual(choose.call_args.args[4], self.directory / "Explicit cache")
            getattr(operations.return_value, args[0]).assert_called_once_with()

    def test_help_exposes_commands_and_acquisition_boundary(self):
        with self.assertRaises(SystemExit) as result:
            dev.main(["--help"])
        self.assertEqual(result.exception.code, 0)
        for name in ("doctor", "setup", "configure", "build", "test", "verify", "all", "clean"):
            self.assertIn(name, self.output.getvalue())
        self.assertIn("existing cache only", self.output.getvalue())


if __name__ == "__main__":
    unittest.main()
