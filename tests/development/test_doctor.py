"""Synthetic doctor fixtures do not certify native ABI or either CI environment."""

import contextlib
import copy
import io
import json
import os
import subprocess
from pathlib import Path
import sys
import tempfile
import unittest
from unittest.mock import patch

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "scripts"))
import dev
from devtools import doctor
from devtools.doctor import (
    BaselineError, check_prerequisites, collect_observations, expected_tools,
    make_selection, policy_entry, read_baseline, select_inputs,
)


LINUX = "linux-x86_64-clang"
WINDOWS = "windows-x86_64-clangcl"


def reviewed_fixture():
    baseline = read_baseline()
    # Historical pending-policy fixtures remain synthetic; tracked CI is now reviewed.
    for entry in baseline["ci_environments"].values():
        entry["status"] = "pending"
    for entry in baseline["platforms"].values():
        entry["status"] = "selected"
        for field, value in entry["native"].items():
            if value is None:
                entry["native"][field] = "1.2.3"
        prefix = "C:\\Fixture Tools\\" if entry["os"] == "Windows" else "/fixture/tools/"
        for field in entry["paths"]:
            entry["paths"][field] = prefix + field
    return baseline


def matching_observations(baseline, profile, environment=None):
    entry = policy_entry(baseline, profile, environment)
    observed = {f"tools.{key}": value for key, value in expected_tools(baseline, entry).items()}
    observed["tools.git"] = "2.55.0.windows.5" if entry["os"] == "Windows" else "2.55.0"
    observed["git_path"] = "C:\\Fixture Tools\\git.exe" if entry["os"] == "Windows" else "/fixture/bin/git"
    observed["tools.clang_cpp"] = observed["tools.clang"]
    observed["compiler_target"] = entry["target"]
    observed.update({f"native.{key}": value for key, value in entry["native"].items() if key != "stdlib_abi"})
    observed.update({f"paths.{key}": value for key, value in entry["paths"].items()})
    if entry["os"] == "Linux":
        observed.update({"native.gxx": entry["native"]["gcc"],
                         "gcc_installation": entry["paths"]["gcc_install_dir"],
                         "stdlib_library": entry["paths"]["stdlib_library"]})
    else:
        observed["native.sdk_runtime_files"] = "present"
    return observed


class DoctorSelectionTests(unittest.TestCase):
    def test_linux_selection_is_not_installed_or_package_readiness(self):
        baseline = read_baseline()
        selected = select_inputs(baseline, LINUX)
        observed = matching_observations(baseline, LINUX)
        observed["tools.conan"] = None
        checks = check_prerequisites(baseline, selected, observed)
        self.assertEqual([(check["identity"], check["status"]) for check in checks if check["status"] != "ok"],
                         [("tools.conan", "missing")])
        self.assertEqual(baseline["backend"]["llvm"], "23.1.2")
        self.assertEqual(baseline["platforms"][LINUX]["tools"]["clang"], "22.1.8")
        self.assertEqual(baseline["platforms"][WINDOWS]["tools"]["clang"], "20.1.8")
        with self.assertRaisesRegex(BaselineError, "not owner-selected"):
            select_inputs(baseline, WINDOWS)

    def test_each_missing_native_identity_and_path_is_rejected(self):
        for profile, entry in reviewed_fixture()["platforms"].items():
            for section in ("native", "paths"):
                for field in entry[section]:
                    baseline = reviewed_fixture()
                    baseline["platforms"][profile][section][field] = None
                    with self.subTest(profile=profile, field=field), self.assertRaisesRegex(BaselineError, field):
                        select_inputs(baseline, profile)

    def test_tool_identity_must_be_exact_per_environment(self):
        for profile in (LINUX, WINDOWS):
            for tool in doctor.TOOLS:
                baseline = reviewed_fixture()
                baseline["platforms"][profile]["tools"][tool] = "pending"
                with self.subTest(profile=profile, tool=tool), self.assertRaisesRegex(BaselineError, f"tools.{tool}"):
                    select_inputs(baseline, profile)
        for version in (">=3.14.7", "3.14.*", "3.x", " pending "):
            baseline = reviewed_fixture()
            baseline["platforms"][LINUX]["tools"]["python"] = version
            with self.subTest(version=version), self.assertRaises(BaselineError):
                select_inputs(baseline, LINUX)

    def test_unsupported_selection_and_ci_substitution_fail(self):
        baseline = reviewed_fixture()
        for profile, config in (("macos-x86_64-clang", "Debug"), (LINUX, "RelWithDebInfo")):
            with self.subTest(profile=profile), self.assertRaises(BaselineError):
                select_inputs(baseline, profile, config)
        for profile, environment in ((LINUX, "ubuntu-ci"), (WINDOWS, "windows-ci")):
            with self.subTest(environment=environment), self.assertRaisesRegex(BaselineError, "developer evidence is not CI evidence"):
                select_inputs(baseline, profile, environment=environment)

    def test_independently_selected_ci_fixture_does_not_overwrite_developer_identity(self):
        baseline = reviewed_fixture()
        ci = copy.deepcopy(baseline["platforms"][LINUX])
        ci.update({"profile": LINUX, "environment": "ubuntu-ci"})
        ci["tools"] = {"python": "3.13.15", "cmake": "3.31.8", "clang": "18.1.3"}
        ci["native"].update({"distribution": "Ubuntu", "distribution_version": "24.04"})
        baseline["ci_environments"]["ubuntu-ci"] = ci
        selected = select_inputs(baseline, LINUX, environment="ubuntu-ci")
        self.assertEqual(selected["environment_name"], "ubuntu-ci")
        self.assertEqual(expected_tools(baseline, policy_entry(baseline, LINUX, "ubuntu-ci"))["python"], "3.13.15")
        self.assertEqual(expected_tools(baseline, policy_entry(baseline, LINUX))["python"], "3.14.7")
        observed = matching_observations(baseline, LINUX)
        checks = check_prerequisites(baseline, selected, observed)
        self.assertTrue(any(check["status"] == "mismatch" for check in checks))

    def test_missing_target_and_wrong_native_settings_fail(self):
        for field, value in (("target", None), ("os", "Windows"), ("arch", "armv8")):
            baseline = reviewed_fixture()
            baseline["platforms"][LINUX][field] = value
            with self.subTest(field=field), self.assertRaises(BaselineError):
                select_inputs(baseline, LINUX)
        baseline = reviewed_fixture()
        baseline["platforms"][LINUX]["native"]["stdlib_abi"] = "libc++"
        with self.assertRaisesRegex(BaselineError, "libstdc"):
            select_inputs(baseline, LINUX)
        for profile, version in ((LINUX, "20.1.8"), (WINDOWS, "22.1.8")):
            baseline = reviewed_fixture()
            baseline["platforms"][profile]["tools"]["clang"] = version
            with self.subTest(profile=profile), self.assertRaisesRegex(BaselineError, "Clang major"):
                select_inputs(baseline, profile)

    def test_malformed_policy_is_reported(self):
        with tempfile.TemporaryDirectory() as directory:
            path = Path(directory) / "baseline.json"
            for content in ('{"schema_version": 99}', "{broken",
                            '{"schema_version": 1, "tools": {}, "platforms": {}, "backend": null}'):
                path.write_text(content, encoding="utf-8")
                with self.subTest(content=content), self.assertRaises(BaselineError):
                    read_baseline(path)
        baseline = reviewed_fixture()
        baseline["platforms"][LINUX]["tools"] = None
        with self.assertRaisesRegex(BaselineError, "own tool identities"):
            select_inputs(baseline, LINUX)

    def test_host_debug_release_and_native_release_build_are_separate(self):
        baseline = reviewed_fixture()
        for profile in (LINUX, WINDOWS):
            for config in ("Debug", "Release"):
                selected = select_inputs(baseline, profile, config)
                with self.subTest(profile=profile, config=config):
                    self.assertEqual(selected["host_profiles"][1].name, config.lower())
                    self.assertEqual(selected["build_profiles"][1].name, "release")
                    self.assertEqual(selected["build_configuration"], "Release")
                    self.assertEqual(selected["host"], selected["target"])
                    self.assertEqual(selected["runtime_type"], config if profile == WINDOWS else None)
        self.assertEqual(select_inputs(baseline, LINUX)["configuration"], "Debug")

    def test_windows_paths_and_versions_do_not_inherit_linux(self):
        baseline = reviewed_fixture()
        selected = select_inputs(baseline, WINDOWS)
        self.assertEqual(selected["host"], "windows-x86_64-msvc")
        self.assertEqual(selected["environment"]["VEYR_C_COMPILER"], "C:\\Fixture Tools\\c_compiler")
        self.assertEqual(selected["environment"]["VEYR_WINDOWS_SDK_VERSION"], "10.0.26100.0")
        self.assertEqual(expected_tools(baseline, baseline["platforms"][WINDOWS])["python"], "3.13.15")
        self.assertEqual(select_inputs(baseline, LINUX)["environment"]["VEYR_GCC_INSTALL_DIR"], "/fixture/tools/gcc_install_dir")


class PrerequisiteTests(unittest.TestCase):
    def test_each_tool_sdk_runtime_and_path_missing_or_mismatched_fails(self):
        baseline = reviewed_fixture()
        for profile in (LINUX, WINDOWS):
            selected = select_inputs(baseline, profile)
            observed = matching_observations(baseline, profile)
            self.assertTrue(all(check["status"] == "ok" for check in check_prerequisites(baseline, selected, observed)))
            for label in observed:
                if label in {"tools.git", "git_path"}:
                    continue  # Git version/path are utility observations, not exact ABI expectations.
                for wrong, status in ((None, "missing"), ("unexpected", "mismatch")):
                    candidate = {**observed, label: wrong}
                    checks = check_prerequisites(baseline, selected, candidate)
                    with self.subTest(profile=profile, label=label, status=status):
                        failed = [check for check in checks if check["identity"] == label][0]
                        self.assertEqual(failed["status"], status)
                        self.assertTrue(failed["remedy"])

    def test_windows_path_case_and_separators_are_equivalent(self):
        baseline = reviewed_fixture()
        observed = matching_observations(baseline, WINDOWS)
        observed["paths.vs_root"] = "c:/fixture tools/VS_ROOT"
        checks = check_prerequisites(baseline, select_inputs(baseline, WINDOWS), observed)
        self.assertTrue(all(check["status"] == "ok" for check in checks))

    def test_ci_selection_is_independent_and_profile_major_matches_reviewed_environment(self):
        baseline = read_baseline()
        for profile, environment, major in ((LINUX, "ubuntu-ci", "18"), (WINDOWS, "windows-ci", "20")):
            for config in ("Debug", "Release"):
                selected = select_inputs(baseline, profile, config, environment=environment)
                self.assertEqual(selected["environment"]["VEYR_CLANG_MAJOR"], major)
                self.assertEqual(selected["build_configuration"], "Release")
                checks = check_prerequisites(baseline, selected, matching_observations(baseline, profile, environment))
                self.assertTrue(all(c["status"] == "ok" for c in checks))
        self.assertEqual(select_inputs(baseline, LINUX)["environment"]["VEYR_CLANG_MAJOR"], "22")
        self.assertEqual(select_inputs(baseline, LINUX, environment="ubuntu-ci")["environment"]["VEYR_GCC_INSTALL_DIR"],
                         "/usr/lib/gcc/x86_64-linux-gnu/13")
        with self.assertRaisesRegex(BaselineError, "not owner-selected"):
            select_inputs(baseline, WINDOWS)

    def test_git_drift_is_observational_but_missing_git_is_actionable(self):
        baseline = read_baseline()
        selected = select_inputs(baseline, WINDOWS, environment="windows-ci")
        observed = matching_observations(baseline, WINDOWS, "windows-ci")
        for version in ("2.55.0.windows.5", "2.56.0.windows.1"):
            observed["tools.git"] = version
            checks = check_prerequisites(baseline, selected, observed)
            self.assertTrue(all(c["status"] == "ok" for c in checks))
            git = next(c for c in checks if c["identity"] == "tools.git")
            self.assertEqual(git["actual"], version)
            self.assertEqual(git["path"], observed["git_path"])
        observed["tools.git"] = None
        git = next(c for c in check_prerequisites(baseline, selected, observed) if c["identity"] == "tools.git")
        self.assertEqual(git["status"], "missing")
        self.assertIn("Make Git available", git["remedy"])

    def test_ci_compiler_and_runtime_drift_still_fails(self):
        baseline = read_baseline()
        for profile, environment, labels in (
            (LINUX, "ubuntu-ci", ("tools.clang", "native.gcc", "native.libc", "native.linker")),
            (WINDOWS, "windows-ci", ("tools.clang", "native.vctools", "native.crt", "native.sdk", "native.linker")),
        ):
            selected = select_inputs(baseline, profile, environment=environment)
            for label in labels:
                observed = matching_observations(baseline, profile, environment)
                observed[label] = "19.1.5" if label == "tools.clang" else "unexpected"
                check = next(c for c in check_prerequisites(baseline, selected, observed) if c["identity"] == label)
                self.assertEqual(check["status"], "mismatch")

    def test_ubuntu_banners_are_read_only_and_unknown_formats_are_rejected(self):
        entry = read_baseline()["ci_environments"]["ubuntu-ci"]
        for libc, linker, accepted in (
            ("ldd (Ubuntu GLIBC 2.39-0ubuntu8.9) 2.39", "GNU ld (GNU Binutils for Ubuntu) 2.42", True),
            ("ldd (GNU libc) 2.44", "GNU ld (GNU Binutils) 2.47", True),
            ("ldd (unknown vendor) 2.39", "LLD 2.42", False),
        ):
            calls = []
            def fake_probe(argv, **kwargs):
                calls.append(argv)
                if argv[0] == "ldd":
                    return libc
                if argv[0] == entry["paths"]["linker"]:
                    return linker
                raise BaselineError("unneeded fixture probe")
            with patch.object(doctor, "_probe", side_effect=fake_probe), \
                 patch.object(doctor, "inspect_conan_tool", return_value="2.33.0"), \
                 patch.object(doctor.shutil, "which", return_value=None), \
                 patch.object(doctor.platform, "freedesktop_os_release", return_value={"NAME": "Ubuntu", "VERSION_ID": "24.04"}), \
                 patch.object(Path, "is_dir", return_value=True), patch.object(Path, "is_file", return_value=True):
                observed = collect_observations(entry)
            if accepted:
                self.assertEqual(observed["native.libc"], "glibc " + libc.split()[-1])
                self.assertEqual(observed["native.linker"], "GNU Binutils " + linker.split()[-1])
            else:
                self.assertIsNone(observed["native.libc"])
                self.assertIsNone(observed["native.linker"])
            self.assertFalse(any("conan" in argv[0] for argv in calls))

    def test_read_only_linux_probes_parse_exact_native_identities(self):
        entry = reviewed_fixture()["platforms"][LINUX]
        commands = []

        def fake_probe(argv, **kwargs):
            commands.append(argv)
            name, flag = Path(argv[0]).name, argv[-1]
            outputs = {
                "cmake": "cmake version 4.4.3", "ninja": "1.13.2", "git": "git version 2.55.0",
                "c_compiler": "clang version 22.1.8", "cpp_compiler": "clang version 22.1.8",
                "linker": "GNU ld (GNU Binutils) 2.47", "ldd": "ldd (GNU libc) 2.44",
            }
            if flag == "-dumpmachine":
                return "x86_64-pc-linux-gnu"
            if flag == "-dumpfullversion":
                return "16.2.1"
            if flag == "-print-libgcc-file-name":
                return entry["paths"]["gcc_install_dir"] + "/libgcc.a"
            if flag == "-print-file-name=libstdc++.so":
                return entry["paths"]["stdlib_library"]
            return outputs[name]

        with patch.object(doctor, "_probe", side_effect=fake_probe), \
             patch.object(doctor, "inspect_conan_tool", side_effect=BaselineError("fixture missing Conan metadata")), \
             patch.object(doctor.shutil, "which", side_effect=lambda name: None if name == "conan" else "/fixture/bin/" + name), \
             patch.object(doctor.platform, "freedesktop_os_release", return_value={"NAME": "Omarchy", "VERSION_ID": "4.0.4"}), \
             patch.object(Path, "is_dir", return_value=True), patch.object(Path, "is_file", return_value=True):
            observed = collect_observations(entry)
        self.assertIsNone(observed["tools.conan"])
        self.assertEqual(observed["native.gcc"], "16.2.1")
        self.assertEqual(observed["native.libc"], "glibc 2.44")
        self.assertEqual(observed["native.linker"], "GNU Binutils 2.47")
        self.assertTrue(all(argv[-1] in {"--version", "-dumpmachine", "-dumpfullversion",
                                        "-print-libgcc-file-name", "-print-file-name=libstdc++.so"} for argv in commands))

    def test_failed_identity_probe_is_not_retried_or_accepted(self):
        entry = reviewed_fixture()["platforms"][LINUX]
        with patch.object(doctor, "_probe", side_effect=BaselineError("fixture probe failed")), \
             patch.object(doctor.shutil, "which", return_value=None), \
             patch.object(doctor.platform, "freedesktop_os_release", return_value={}):
            observed = collect_observations(entry)
        self.assertIsNone(observed["tools.clang"])
        self.assertIn("fixture probe failed", observed["errors"]["tools.clang"])

    def test_conan_metadata_inspection_does_not_receive_home_or_search_path(self):
        entry = reviewed_fixture()["platforms"][LINUX]
        with patch.object(doctor, "inspect_conan_tool", return_value="2.33.0") as inspect, \
             patch.object(doctor, "_probe", side_effect=BaselineError("fixture unused tools")), \
             patch.object(doctor.shutil, "which", return_value=None) as which, \
             patch.object(doctor.platform, "freedesktop_os_release", return_value={}):
            observed = collect_observations(entry, conan_home="/fixture/project cache")
        inspect.assert_called_once_with(doctor.conan_tool_paths("Linux")[1], "3.14.7")
        self.assertNotIn("conan", [call.args[0] for call in which.call_args_list])
        self.assertEqual(observed["tools.conan"], "2.33.0")


class CIObservationCompatibilityTests(unittest.TestCase):
    """Replay CI identities with all commands/files mocked; no native acceptance."""

    def observe(self, environment, config="Debug", ninja="1.13.2.git.kitware.jobserver-pipe-1",
                target=None, linker="14.44.35229.0", linker_failure=False, missing_linker=False):
        baseline = read_baseline()
        entry = baseline["ci_environments"][environment]
        paths = entry["paths"]
        target = entry["target"] if target is None else target
        calls = []
        crt = "\n".join(f"#define _VC_CRT_{part}_VERSION {number}" for part, number in
                        zip(("MAJOR", "MINOR", "BUILD", "RBUILD"), entry["native"].get("crt", "0.0.0.0").split(".")))

        def fake_probe(argv, **kwargs):
            calls.append((argv, kwargs))
            if argv[0] in {"cmake", "ninja", "git"}:
                return {"cmake": "cmake version " + entry["tools"]["cmake"],
                        "ninja": ninja, "git": "git version 2.56.0.windows.1"}[argv[0]]
            if argv[0] in {paths["c_compiler"], paths["cpp_compiler"]}:
                if argv[-1] == "--version":
                    return "clang version " + entry["tools"]["clang"] + (f"\nTarget: {target}\nThread model: posix" if target else "")
                self.assertEqual(entry["os"], "Linux", "clang-cl must not receive -dumpmachine")
                self.assertEqual(argv[-1], "-dumpmachine")
                return target
            if argv[0].endswith("vswhere.exe"):
                return json.dumps([{"installationPath": paths["vs_root"],
                                    "installationVersion": entry["native"]["vs_installation"]}])
            if argv[0].endswith("powershell.exe"):
                self.assertEqual(argv[0], r"C:\Windows\System32\WindowsPowerShell\v1.0\powershell.exe")
                self.assertEqual(argv[1:5], ["-NoLogo", "-NoProfile", "-NonInteractive", "-Command"])
                self.assertIn("FileVersionInfo]::GetVersionInfo($env:VEYR_DOCTOR_VERSION_FILE).FileVersion", argv[-1])
                self.assertNotIn(paths["linker"], argv[-1])  # Path is data, never PowerShell source.
                self.assertEqual(kwargs["env"]["VEYR_DOCTOR_VERSION_FILE"], paths["linker"])
                if linker_failure:
                    raise BaselineError("fixture metadata query failed")
                return linker
            self.assertEqual(entry["os"], "Linux", "Windows linker must never be launched")
            outputs = {
                ("gcc", "-dumpfullversion"): entry["native"]["gcc"],
                ("g++", "-dumpfullversion"): entry["native"]["gcc"],
                ("ldd", "--version"): "ldd (Ubuntu GLIBC 2.39-0ubuntu8.9) 2.39",
                (paths["linker"], "--version"): "GNU ld (GNU Binutils for Ubuntu) 2.42",
                ("gcc", "-print-libgcc-file-name"): paths["gcc_install_dir"] + "/libgcc.a",
                ("g++", "-print-file-name=libstdc++.so"): paths["stdlib_library"],
            }
            return outputs[tuple(argv)]

        shell = {"SystemRoot": r"C:\Windows", "ProgramFiles(x86)": r"C:\Program Files (x86)"}
        if entry["os"] == "Windows":
            shell.update({"VSINSTALLDIR": paths["vs_root"] + "\\",
                          "VCToolsInstallDir": paths["vctools_root"] + "\\",
                          "WindowsSdkDir": paths["sdk_root"] + "\\",
                          "VCToolsVersion": entry["native"]["vctools"],
                          "WindowsSDKVersion": entry["native"]["sdk"] + "\\"})
        with patch.object(doctor, "_probe", side_effect=fake_probe), \
             patch.object(doctor, "inspect_conan_tool", return_value="2.33.0") as inspect, \
             patch.object(doctor.platform, "python_version", return_value=entry["tools"]["python"]), \
             patch.object(doctor.platform, "freedesktop_os_release", return_value={"NAME": "Ubuntu", "VERSION_ID": "24.04"}), \
             patch.object(doctor.shutil, "which", side_effect=lambda name: name), \
             patch.object(Path, "is_dir", return_value=True), \
             patch.object(Path, "is_file", side_effect=lambda path: not (missing_linker and str(path) == paths["linker"]), autospec=True), \
             patch.object(Path, "read_text", return_value=crt), \
             patch.object(Path, "mkdir") as mkdir, patch.object(Path, "write_text") as write, \
             patch.object(doctor.subprocess, "run") as native_run, \
             patch.dict(os.environ, shell, clear=True):
            observed = collect_observations(entry, config)
        mkdir.assert_not_called()
        write.assert_not_called()
        native_run.assert_not_called()
        inspect.assert_called_once_with(doctor.conan_tool_paths(entry["os"])[1], entry["tools"]["python"])
        selected = select_inputs(baseline, entry["profile"], config, environment=environment)
        checks = check_prerequisites(baseline, selected, observed)
        return observed, {check["identity"]: check for check in checks}, calls

    def test_all_four_ci_prerequisite_replays_pass_with_vendor_ninja_and_read_only_queries(self):
        for environment in ("ubuntu-ci", "windows-ci"):
            for config in ("Debug", "Release"):
                with self.subTest(environment=environment, config=config):
                    observed, checks, calls = self.observe(environment, config)
                    self.assertTrue(all(check["status"] == "ok" for check in checks.values()), checks)
                    self.assertEqual(observed["tools.ninja"], "1.13.2")
                    self.assertEqual(observed["tools.ninja.raw"], "1.13.2.git.kitware.jobserver-pipe-1")
                    self.assertIn(observed["tools.ninja.raw"], checks["tools.ninja"]["detail"])
                    self.assertEqual(sum(argv == ["ninja", "--version"] for argv, _ in calls), 1)
                    if environment == "windows-ci":
                        self.assertFalse(any("-dumpmachine" in argv or "/?" in argv for argv, _ in calls))
                        self.assertEqual(observed["native.linker"], "14.44.35229.0")

    def test_ninja_semantic_version_is_exact_and_malformed_banners_fail(self):
        for banner, status in (("1.13.2", "ok"), ("1.13.2.git.kitware.jobserver-pipe-1", "ok"),
                               ("1.13.1.git.kitware.jobserver-pipe-1", "mismatch"),
                               ("1.13.3", "mismatch"), ("1.13.20.git.kitware", "mismatch"),
                               ("1.13.2.1", "missing"), ("1.13.2-rc1", "missing"),
                               ("ninja 1.13.2", "missing"), ("1.13.2garbage", "missing"),
                               ("1.13.2\n1.14.0", "missing")):
            for environment in ("ubuntu-ci", "windows-ci"):
                with self.subTest(banner=banner, environment=environment):
                    observed, checks, _ = self.observe(environment, ninja=banner)
                    self.assertEqual(checks["tools.ninja"]["status"], status)
                    self.assertEqual(observed["tools.ninja.raw"], banner)
                    self.assertIn(banner, checks["tools.ninja"]["detail"])

    def test_windows_target_banner_must_report_exact_native_triple(self):
        for target, status in (("aarch64-pc-windows-msvc", "mismatch"),
                               ("x86_64-pc-linux-gnu", "mismatch"), ("", "missing")):
            with self.subTest(target=target):
                observed, checks, calls = self.observe("windows-ci", target=target)
                self.assertEqual(checks["compiler_target"]["status"], status)
                self.assertEqual(checks["tools.clang"]["status"], "ok")
                self.assertIn("clang version 20.1.8", observed["compiler_target.raw"])
                self.assertFalse(any("-dumpmachine" in argv for argv, _ in calls))

    def test_windows_linker_metadata_is_exact_and_failures_are_not_retried(self):
        for version, status in (("14.44.35229.1", "mismatch"), ("14.43.35229.0", "mismatch"),
                                ("14.44.35229", "missing"), ("", "missing"),
                                ("14.44.35229.0 vendor", "missing")):
            with self.subTest(version=version):
                _, checks, calls = self.observe("windows-ci", linker=version)
                self.assertEqual(checks["native.linker"]["status"], status)
                self.assertEqual(sum(argv[0].endswith("powershell.exe") for argv, _ in calls), 1)
        _, checks, calls = self.observe("windows-ci", linker_failure=True)
        self.assertEqual(checks["native.linker"]["status"], "missing")
        self.assertIn("fixture metadata query failed", checks["native.linker"]["detail"])
        self.assertEqual(sum(argv[0].endswith("powershell.exe") for argv, _ in calls), 1)
        _, checks, _ = self.observe("windows-ci", missing_linker=True)
        self.assertEqual(checks["paths.linker"]["status"], "missing")


class BuildSelectionTests(unittest.TestCase):
    def setUp(self):
        self.baseline = reviewed_fixture()
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name) / "Project with spaces"
        self.root.mkdir()
        self.cache = Path(self.temporary.name) / "Conan home"

    def selection(self, config="Debug", provider="managed", **kwargs):
        selected = select_inputs(self.baseline, LINUX, config, root=self.root)
        with patch.object(doctor, "_probe", return_value=str(self.root / ".git")):
            return make_selection(self.baseline, selected, provider=provider,
                                  root=self.root, conan_home=self.cache, **kwargs)

    def test_build_dirs_isolate_configuration_provider_and_tool_identity(self):
        debug, release = self.selection(), self.selection("Release")
        self.assertNotEqual(debug.build_dir, release.build_dir)
        config = Path(self.temporary.name) / "External LLVM with spaces"
        config.mkdir()
        (config / "LLVMConfig.cmake").write_text("# fixture", encoding="utf-8")
        external = self.selection(provider="custom", llvm_dir=config)
        self.assertNotEqual(debug.build_dir, external.build_dir)
        self.baseline["platforms"][LINUX]["tools"]["cmake"] = "4.4.4"
        self.assertNotEqual(debug.build_dir, self.selection().build_dir)
        self.assertFalse(debug.build_dir.exists())
        self.assertFalse(self.cache.exists())
        self.assertEqual(debug.lockfile.name, "linux-x86_64-clang-Debug.lock")
        self.assertEqual(debug.conan_executable, self.root / ".veyr-dev/tools/conan-2.33.0/bin/conan")

    def test_git_observation_does_not_change_build_identity(self):
        before = self.selection().build_dir
        self.baseline["platforms"][LINUX]["git"] = "2.56.0"
        self.assertEqual(self.selection().build_dir, before)
        self.baseline["platforms"][LINUX]["observed_candidate"] = {"git": "2.57.0", "image_version": "new"}
        self.assertEqual(self.selection().build_dir, before)

    def test_missing_external_config_and_implicit_fallback_fail(self):
        for provider in ("system", "custom"):
            with self.subTest(provider=provider), self.assertRaisesRegex(BaselineError, "LLVMConfig"):
                self.selection(provider=provider)
        with self.assertRaisesRegex(BaselineError, "--llvm-dir"):
            self.selection(llvm_dir=self.root)
        with self.assertRaisesRegex(BaselineError, "native Release"):
            self.selection(build_profile=WINDOWS)

    def test_home_in_checkout_is_rejected_and_defaults_use_project_common_dir(self):
        selected = select_inputs(self.baseline, LINUX, root=self.root)
        with patch.object(doctor, "_probe", return_value=str(self.root / ".git")):
            for home in (self.root, self.root / "cache"):
                with self.subTest(home=home), self.assertRaisesRegex(BaselineError, "outside"):
                    make_selection(self.baseline, selected, conan_home=home, root=self.root)
            with self.assertRaisesRegex(BaselineError, "shared .conan2"):
                make_selection(self.baseline, selected, conan_home=Path.home() / ".conan2", root=self.root)
            with patch.dict(os.environ, {"XDG_CACHE_HOME": str(self.cache), "CONAN_HOME": str(self.root)}, clear=True):
                first = make_selection(self.baseline, selected, root=self.root)
                second = make_selection(self.baseline, selected, root=self.root / "other-worktree")
        self.assertEqual(first.conan_home, second.conan_home)
        self.assertIn("veyr", first.conan_home.parts)
        self.assertNotIn(".conan2", first.conan_home.parts)
        self.assertFalse(first.conan_home.exists())


class ConanBootstrapTests(unittest.TestCase):
    """All tool execution mocked; temporary fixtures never contain a real venv."""

    def setUp(self):
        self.temporary = tempfile.TemporaryDirectory()
        self.addCleanup(self.temporary.cleanup)
        self.root = Path(self.temporary.name) / "Project with spaces"
        self.root.mkdir()
        self.baseline = reviewed_fixture()
        self.entry = self.baseline["platforms"][LINUX]
        selected = select_inputs(self.baseline, LINUX, root=self.root)
        with patch.object(doctor, "_probe", return_value=str(self.root / ".git")):
            self.selection = make_selection(self.baseline, selected, root=self.root,
                                            conan_home=Path(self.temporary.name) / "Package cache")
        self.executable = self.selection.conan_executable
        self.directory = self.executable.parent.parent
        self.commands = []
        self.probes = []

    def write_environment(self, system="Linux", isolated=True):
        python, executable = doctor.conan_tool_paths(system, self.root)
        executable.parent.mkdir(parents=True, exist_ok=True)
        (executable.parent.parent / "pyvenv.cfg").write_text(
            f"include-system-site-packages = {'false' if isolated else 'true'}\n", encoding="utf-8")
        python.write_text("# synthetic Python", encoding="utf-8")
        executable.write_text("# synthetic Conan", encoding="utf-8")
        return executable

    def probe(self, argv, **kwargs):
        self.probes.append((argv, kwargs))
        self.assertNotIn(Path(argv[0]).name, {"conan", "conan.exe"})
        return json.dumps({"version": "3.14.7", "prefix": str(self.directory),
                           "base_prefix": str(self.root / "Reviewed Python"),
                           "conan_version": "2.33.0",
                           "conan_location": str(self.directory / "lib" / "site-packages")})

    def fake_run(self, argv, **kwargs):
        self.commands.append((argv, kwargs))
        if "venv" in argv:
            self.write_environment()
        return subprocess.CompletedProcess(argv, 0, stdout="Conan version 2.33.0\n" if argv[-1] == "--version" else None)

    def bootstrap(self, run=None, probe=None):
        with patch.object(dev.platform, "python_version", return_value="3.14.7"), \
             patch.object(dev.sys, "prefix", "/reviewed/python"), \
             patch.object(dev.sys, "base_prefix", "/reviewed/python"), \
             patch.object(dev.sys, "executable", str(self.root / "Reviewed Python" / "python")), \
             patch.object(dev.subprocess, "run", side_effect=run or self.fake_run), \
             patch.object(doctor, "_probe", side_effect=probe or self.probe), \
             contextlib.redirect_stdout(io.StringIO()):
            return dev.bootstrap_conan(self.selection, self.entry)

    def test_missing_environment_bootstraps_exact_isolated_cli_and_checks(self):
        self.assertEqual(self.bootstrap(), "created")
        argv = [call[0] for call in self.commands]
        self.assertEqual(len(argv), 4)
        self.assertEqual(argv[0], [str(self.root / "Reviewed Python" / "python"),
                                   "-I", "-m", "venv", str(self.directory)])
        self.assertEqual(argv[1], [str(self.executable.with_name("python")), "-I", "-m", "pip",
                                   "--isolated", "--require-virtualenv", "--disable-pip-version-check",
                                   "install", "--index-url", "https://pypi.org/simple", "--no-cache-dir",
                                   "--only-binary=:all:", "conan==2.33.0"])
        self.assertEqual(argv[2], [str(self.executable), "--version"])
        self.assertEqual(argv[3][-1], "check")
        for command, kwargs in self.commands:
            self.assertTrue(Path(command[0]).is_absolute())
            self.assertEqual(kwargs["env"]["CONAN_HOME"], str(self.selection.conan_home))
            self.assertNotIn("PYTHONPATH", kwargs["env"])
        self.assertEqual(self.commands[1][1]["env"]["PIP_CONFIG_FILE"], os.devnull)
        self.assertEqual(len(self.probes), 1)
        self.assertEqual(self.probes[-1][0][1:4], ["-I", "-B", "-c"])
        self.assertNotIn("CONAN_HOME", self.probes[-1][1]["env"])
        self.assertEqual(self.probes[-1][1]["env"]["PYTHONDONTWRITEBYTECODE"], "1")
        self.assertFalse(self.selection.conan_home.exists())

    def test_valid_environment_reused_without_install_or_upgrade(self):
        self.write_environment()
        self.assertEqual(self.bootstrap(), "reused")
        self.assertEqual(len(self.commands), 2)
        self.assertEqual(self.commands[0][0], [str(self.executable), "--version"])
        self.assertEqual(self.commands[1][0][-1], "check")
        for argv, _ in self.commands:
            self.assertNotIn("install", argv)
            self.assertNotIn("--upgrade", argv)

    def test_broken_or_nonisolated_environment_fails_without_mutation(self):
        for state in ("missing-config", "missing-executable", "system-site"):
            with self.subTest(state=state):
                self.write_environment(isolated=state != "system-site")
                if state == "missing-config":
                    (self.directory / "pyvenv.cfg").unlink()
                if state == "missing-executable":
                    self.executable.unlink()
                before = {str(path): path.read_bytes() for path in self.directory.rglob("*") if path.is_file()}
                with self.assertRaisesRegex(BaselineError, "Expected isolated Conan 2.33.0") as error:
                    self.bootstrap()
                self.assertIn(str(self.directory), str(error.exception))
                self.assertIn("remove only the exact", str(error.exception))
                self.assertEqual(self.commands, [])
                self.assertEqual(before, {str(path): path.read_bytes() for path in self.directory.rglob("*") if path.is_file()})

    def test_wrong_conan_version_reports_expected_actual_without_repair(self):
        self.write_environment()
        def wrong_version(argv, **kwargs):
            result = json.loads(self.probe(argv, **kwargs))
            result["conan_version"] = "2.32.0"
            return json.dumps(result)
        with self.assertRaisesRegex(BaselineError, "expected=2.33.0; actual=2.32.0"):
            self.bootstrap(probe=wrong_version)
        self.assertEqual(self.commands, [])
        self.assertTrue(self.executable.is_file())

    def test_doctor_metadata_probe_never_launches_conan_or_touches_home(self):
        self.write_environment()
        home = self.selection.conan_home
        for state in ("absent", "unwritable-file", "existing"):
            with self.subTest(state=state):
                if state == "unwritable-file":
                    home.write_bytes(b"not a writable directory")
                    home.chmod(0o444)
                elif state == "existing":
                    home.chmod(0o644)
                    home.unlink()
                    home.mkdir()
                    (home / "version.txt").write_bytes(b"unchanged home metadata")
                before = {str(p.relative_to(home)): (p.read_bytes(), p.stat().st_mtime_ns)
                          for p in home.rglob("*") if p.is_file()} if home.is_dir() else None
                with patch.object(doctor, "_probe", side_effect=self.probe), \
                     patch.dict(os.environ, {"CONAN_HOME": str(home), "PYTHONPATH": "/unrelated"}), \
                     patch.object(dev.subprocess, "run") as run:
                    version = doctor.inspect_conan_tool(self.executable, "3.14.7")
                run.assert_not_called()
                self.assertEqual(version, "2.33.0")
                argv, kwargs = self.probes[-1]
                self.assertEqual(argv[0], str(self.executable.with_name("python")))
                self.assertEqual(argv[1:4], ["-I", "-B", "-c"])
                self.assertIn("from importlib.metadata import distribution", argv[-1])
                self.assertNotIn("import conan", argv[-1])
                self.assertNotIn("CONAN_HOME", kwargs["env"])
                self.assertNotIn("PYTHONPATH", kwargs["env"])
                if state == "absent":
                    self.assertFalse(home.exists())
                elif state == "unwritable-file":
                    self.assertEqual(home.read_bytes(), b"not a writable directory")
                else:
                    after = {str(p.relative_to(home)): (p.read_bytes(), p.stat().st_mtime_ns)
                             for p in home.rglob("*") if p.is_file()}
                    self.assertEqual(before, after)
        self.assertEqual(self.commands, [])

    def test_doctor_reports_wrong_metadata_version_as_mismatch(self):
        self.write_environment()
        def probe(argv, **kwargs):
            if Path(argv[0]).name == "python":
                result = json.loads(self.probe(argv, **kwargs))
                result["conan_version"] = "2.32.0"
                return json.dumps(result)
            raise BaselineError("fixture other native identities")
        with patch.object(doctor, "_probe", side_effect=probe), \
             patch.object(doctor.shutil, "which", return_value=None), \
             patch.object(doctor.platform, "freedesktop_os_release", return_value={}):
            observed = collect_observations(self.entry, conan_home=self.selection.conan_home,
                                            conan_executable=self.executable)
        checks = check_prerequisites(self.baseline, select_inputs(self.baseline, LINUX), observed)
        conan = next(check for check in checks if check["identity"] == "tools.conan")
        self.assertEqual(conan["actual"], "2.32.0")
        self.assertEqual(conan["expected"], "2.33.0")
        self.assertEqual(conan["status"], "mismatch")

    def test_doctor_cli_uses_metadata_with_unwritable_home_without_setup(self):
        self.write_environment()
        home = self.selection.conan_home
        home.write_bytes(b"unwritable Conan home fixture")
        home.chmod(0o444)
        self.addCleanup(home.chmod, 0o644)
        output = io.StringIO()
        def probe(argv, **kwargs):
            if Path(argv[0]).name == "python":
                return self.probe(argv, **kwargs)
            raise BaselineError("fixture other native prerequisites unavailable")
        with patch.object(dev, "read_baseline", return_value=self.baseline), \
             patch.object(dev, "make_selection", return_value=self.selection), \
             patch.object(dev, "bootstrap_conan") as setup, \
             patch.object(doctor, "_probe", side_effect=probe), \
             patch.object(doctor.shutil, "which", return_value=None), \
             patch.object(doctor.platform, "freedesktop_os_release", return_value={}), \
             patch.object(dev.platform, "system", return_value="Linux"), \
             patch.object(dev.platform, "machine", return_value="x86_64"), \
             patch.object(dev.subprocess, "run") as run, \
             patch.dict(os.environ, {"CONAN_HOME": str(home)}, clear=True), \
             contextlib.redirect_stdout(output):
            result = dev.main(["doctor", "--json", "--conan-home", str(home)])
        report = json.loads(output.getvalue())
        conan = next(check for check in report["checks"] if check["identity"] == "tools.conan")
        self.assertEqual(conan["status"], "ok")
        self.assertEqual(conan["actual"], "2.33.0")
        self.assertEqual(report["selection"]["conan_executable"], str(self.executable))
        self.assertEqual(result, 1)  # Other synthetic host prerequisites are unavailable.
        setup.assert_not_called()
        run.assert_not_called()
        self.assertEqual(home.read_bytes(), b"unwritable Conan home fixture")

    def test_missing_broken_or_outside_metadata_fails_without_cli_or_repair(self):
        self.write_environment()
        def missing(*args, **kwargs):
            raise BaselineError("PackageNotFoundError: No package metadata was found for conan")
        for probe, message in ((missing, "PackageNotFoundError"),
                               (lambda *args, **kwargs: "broken JSON", "actual="),
                               (lambda *args, **kwargs: json.dumps({
                                   "version": "3.14.7", "prefix": str(self.directory), "base_prefix": "/base",
                                   "conan_version": "2.33.0", "conan_location": "/unrelated/site-packages"}), "distribution must be inside")):
            with self.subTest(message=message), patch.object(doctor, "_probe", side_effect=probe) as run:
                with self.assertRaisesRegex(BaselineError, message):
                    doctor.inspect_conan_tool(self.executable, "3.14.7")
                self.assertEqual(run.call_count, 1)
                self.assertEqual(Path(run.call_args.args[0][0]).name, "python")
        self.assertEqual(self.commands, [])

    def test_setup_rejects_cli_mismatch_after_successful_metadata_validation(self):
        self.write_environment()
        def mismatched_cli(argv, **kwargs):
            self.commands.append((argv, kwargs))
            return subprocess.CompletedProcess(argv, 0, stdout="Conan version 2.32.0")
        with self.assertRaisesRegex(BaselineError, "CLI launch version mismatch: expected=2.33.0; actual=2.32.0"):
            self.bootstrap(run=mismatched_cli)
        self.assertEqual(len(self.probes), 1)
        self.assertEqual(len(self.commands), 1)
        self.assertEqual(self.commands[0][0], [str(self.executable), "--version"])
        self.assertEqual(self.commands[0][1]["env"]["CONAN_HOME"], str(self.selection.conan_home))

    def test_existing_wrong_python_or_prefix_fails_before_pip(self):
        self.write_environment()
        for runtime in ({"version": "3.13.15", "prefix": str(self.directory), "base_prefix": "/base"},
                        {"version": "3.14.7", "prefix": "/unrelated", "base_prefix": "/base"},
                        {"version": "3.14.7", "prefix": str(self.directory), "base_prefix": str(self.directory)}):
            with self.subTest(runtime=runtime), self.assertRaisesRegex(BaselineError, "expected isolated Python 3.14.7"):
                self.bootstrap(probe=lambda *args, **kwargs: json.dumps(runtime))
        self.assertEqual(self.commands, [])

    def test_bootstrap_install_and_pip_check_fail_once_without_retry(self):
        for failing_step in ("venv", "install", "--version", "check"):
            with self.subTest(step=failing_step), tempfile.TemporaryDirectory() as directory:
                # Use a fresh missing location for each failure path.
                self.root = Path(directory)
                selected = select_inputs(self.baseline, LINUX, root=self.root)
                with patch.object(doctor, "_probe", return_value=str(self.root / ".git")):
                    self.selection = make_selection(self.baseline, selected, root=self.root,
                                                    conan_home=Path(self.temporary.name) / "Package cache")
                self.executable = self.selection.conan_executable
                self.directory = self.executable.parent.parent
                self.commands = []
                def fail(argv, **kwargs):
                    if failing_step in argv:
                        self.commands.append((argv, kwargs))
                        raise subprocess.CalledProcessError(1, argv)
                    return self.fake_run(argv, **kwargs)
                with self.assertRaisesRegex(BaselineError, "setup stopped"):
                    self.bootstrap(run=fail)
                self.assertEqual(len(self.commands), {"venv": 1, "install": 2, "--version": 3, "check": 4}[failing_step])

    def test_unrelated_conan_on_path_is_ignored_and_doctor_is_read_only(self):
        with patch.object(doctor.shutil, "which", return_value="/unrelated/conan") as which, \
             patch.object(doctor, "_probe", side_effect=BaselineError("fixture unrelated tools")), \
             patch.object(doctor.platform, "freedesktop_os_release", return_value={}), \
             patch.object(dev.subprocess, "run") as run:
            observed = collect_observations(self.entry, conan_home=self.selection.conan_home,
                                            conan_executable=self.executable)
        self.assertIsNone(observed["tools.conan"])
        self.assertNotIn("conan", [call.args[0] for call in which.call_args_list])
        run.assert_not_called()
        self.assertFalse(self.directory.exists())

    def test_wrong_base_python_and_redirected_environment_fail_before_install(self):
        with patch.object(dev.platform, "python_version", return_value="3.13.15"), \
             patch.object(dev.subprocess, "run") as run:
            with self.assertRaisesRegex(BaselineError, "reviewed base Python 3.14.7"):
                dev.bootstrap_conan(self.selection, self.entry)
        run.assert_not_called()
        self.directory.parent.mkdir(parents=True)
        try:
            self.directory.symlink_to(self.root, target_is_directory=True)
        except OSError:
            self.skipTest("Creating symlinks unavailable on this host")
        with self.assertRaisesRegex(BaselineError, "redirected path"):
            self.bootstrap()
        self.assertEqual(self.commands, [])

    def test_windows_paths_and_read_only_identity_use_project_executable(self):
        executable = self.write_environment("Windows")
        self.assertEqual(executable, self.root / ".veyr-dev/tools/conan-2.33.0/Scripts/conan.exe")
        self.assertEqual(doctor.conan_tool_paths("Windows", self.root)[0].name, "python.exe")
        with patch.object(doctor, "_probe", side_effect=self.probe):
            actual = doctor.inspect_conan_tool(executable, "3.14.7")
        self.assertEqual(actual, "2.33.0")
        self.assertEqual(self.probes[-1][0][0], str(executable.with_name("python.exe")))
        self.assertEqual(len(self.probes), 1)

    def test_tools_only_cli_stops_before_doctor_and_all_package_work(self):
        output = io.StringIO()
        with patch.object(dev, "read_baseline", return_value=self.baseline), \
             patch.object(dev, "make_selection", return_value=self.selection), \
             patch.object(dev.platform, "python_version", return_value="3.14.7"), \
             patch.object(dev.sys, "prefix", "/reviewed/python"), \
             patch.object(dev.sys, "base_prefix", "/reviewed/python"), \
             patch.object(dev.subprocess, "run", side_effect=self.fake_run), \
             patch.object(doctor, "_probe", side_effect=self.probe), \
             patch.object(dev, "collect_observations") as collect, \
             patch.object(dev.platform, "system", return_value="Linux"), \
             patch.object(dev.platform, "machine", return_value="x86_64"), \
             patch.dict(os.environ, {}, clear=True), contextlib.redirect_stdout(output):
            result = dev.main(["setup", "--tools-only"])
        self.assertEqual(result, 0)
        collect.assert_not_called()
        self.assertIn("No Conan dependency/package operation", output.getvalue())
        for argv, _ in self.commands:
            if Path(argv[0]).name in {"conan", "conan.exe"}:
                self.assertEqual(argv[1:], ["--version"])
        self.assertFalse(self.selection.conan_home.exists())

    def test_plain_setup_delegates_to_dependency_plan(self):
        with patch.object(dev, "read_baseline", return_value=self.baseline), \
             patch.object(dev, "make_selection", return_value=self.selection), \
             patch.object(dev.platform, "system", return_value="Linux"), \
             patch.object(dev.platform, "machine", return_value="x86_64"), \
             patch.object(dev, "setup_dependencies") as setup, \
             patch.dict(os.environ, {}, clear=True), \
             contextlib.redirect_stdout(io.StringIO()):
            result = dev.main(["setup"])
        self.assertEqual(result, 0)
        setup.assert_called_once()
        self.assertIs(setup.call_args.args[0], self.selection)
        self.assertFalse(setup.call_args.kwargs["initialize_lock"])
        self.assertTrue(callable(setup.call_args.kwargs["ensure_prerequisites"]))


class DoctorCliTests(unittest.TestCase):
    def run_cli(self, argv, baseline=None, missing=None, system="Linux", environ=None):
        baseline = baseline or reviewed_fixture()
        profile = LINUX if system == "Linux" else WINDOWS
        observed = matching_observations(baseline, profile)
        if missing:
            observed[missing] = None
        output, errors = io.StringIO(), io.StringIO()
        with patch.object(dev, "read_baseline", return_value=baseline), \
             patch.object(dev, "collect_observations", return_value=observed) as collect, \
             patch.object(dev.platform, "system", return_value=system), \
             patch.object(dev.platform, "machine", return_value="AMD64" if system == "Windows" else "x86_64"), \
             patch.object(doctor, "_probe", return_value=str(doctor.ROOT / ".git")), \
             patch.dict(os.environ, environ or {}, clear=True), \
             contextlib.redirect_stdout(output), contextlib.redirect_stderr(errors):
            result = dev.main(argv)
        return result, output.getvalue(), errors.getvalue(), collect

    def test_missing_conan_reports_remedy_nonzero_and_does_not_claim_package_readiness(self):
        result, output, _, _ = self.run_cli(["doctor", "--json"], missing="tools.conan")
        report = json.loads(output)
        self.assertEqual(result, 1)
        self.assertFalse(report["prerequisites_ready"])
        self.assertEqual(report["package_readiness"], "unverified")
        conan = [check for check in report["checks"] if check["identity"] == "tools.conan"][0]
        self.assertIn("2.33.0", conan["remedy"])
        self.assertEqual(report["selection"]["config"], "Debug")

    def test_success_certifies_only_prerequisites_and_release_override_is_printed(self):
        result, output, _, _ = self.run_cli(["doctor", "--config", "Release"])
        self.assertEqual(result, 0)
        self.assertIn("config=Release", output)
        self.assertIn("Native ABI/package compatibility: unverified", output)

    def test_windows_developer_identity_is_independent_fixture_only(self):
        with tempfile.TemporaryDirectory() as directory:
            result, output, _, _ = self.run_cli(["doctor", "--json", "--conan-home", directory], system="Windows")
        self.assertEqual(result, 0)
        report = json.loads(output)
        self.assertEqual(report["selection"]["host"], "windows-x86_64-msvc")
        python = [check for check in report["checks"] if check["identity"] == "tools.python"][0]
        self.assertEqual(python["expected"], "3.13.15")

    def test_ci_and_foreign_native_profile_fail_before_any_tool_probe(self):
        for args, env in ((["doctor", "--environment", "ubuntu-ci"], {}),
                          (["doctor"], {"GITHUB_ACTIONS": "true"}),
                          (["doctor", "--environment", "linux-developer"], {"GITHUB_ACTIONS": "true"}),
                          (["doctor", "--profile", WINDOWS], {})):
            with self.subTest(args=args, env=env):
                result, _, _, collect = self.run_cli(args, environ=env)
                self.assertEqual(result, 2)
                collect.assert_not_called()

    def test_pending_windows_and_mac_os_fail_actionably(self):
        with tempfile.TemporaryDirectory() as directory:
            result, output, _, collect = self.run_cli(["doctor", "--json", "--conan-home", directory],
                                                      baseline=read_baseline(), system="Windows")
        self.assertEqual(result, 2)
        self.assertIn("native evidence is pending", output)
        collect.assert_not_called()
        result, _, error, collect = self.run_cli(["doctor"], system="Darwin")
        self.assertEqual(result, 2)
        self.assertIn("macOS is deferred", error)
        collect.assert_not_called()


if __name__ == "__main__":
    unittest.main()
