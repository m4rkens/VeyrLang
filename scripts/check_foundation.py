"""Native foundation task checks. Never setup/acquire; failed output is retained."""

import argparse
import ast
import contextlib
from dataclasses import replace
import hashlib
import json
import os
from pathlib import Path
import platform
import re
import stat
import subprocess
import sys
import tempfile

from dev import Operations
from devtools.conan_ops import conan_home_guard
from devtools.doctor import (
    ROOT, PROFILES, BaselineError, read_baseline, select_inputs, make_selection,
    policy_entry, collect_observations, check_prerequisites,
)
from devtools.runner import run_command


def input_identity(selection, selected, root):
    paths = {selection.lockfile, root / "scripts/devtools/baseline.json",
             *selected["host_profiles"], *selected["build_profiles"]}
    return {str(path): hashlib.sha256(path.read_bytes()).hexdigest() for path in sorted(paths)}


def tree_identity(root):
    """Read-only metadata digest; exclude atime, never follow links/junctions."""
    digest, count = hashlib.sha256(), 0

    def visit(path):
        nonlocal count
        info = path.lstat()
        record = [str(path.relative_to(root)), info.st_mode, info.st_size,
                  info.st_mtime_ns, info.st_ctime_ns, info.st_ino, info.st_dev,
                  info.st_nlink, info.st_uid, info.st_gid,
                  os.readlink(path) if path.is_symlink() else None]
        digest.update(json.dumps(record, separators=(",", ":")).encode() + b"\n")
        count += 1
        if stat.S_ISDIR(info.st_mode) and not path.is_junction():
            with os.scandir(path) as entries:
                children = sorted(Path(child.path) for child in entries)
            for child in children:
                visit(child)

    if not os.path.lexists(root):
        return {"entries": 0, "metadata_sha256": None}
    visit(root)
    return {"entries": count, "metadata_sha256": digest.hexdigest()}


def package_identities(data):
    """Compare Conan's own raw list output; this is no package graph/lock."""
    try:
        local = data["Local Cache"]
        if not isinstance(local, dict) or "error" in local:
            raise ValueError("missing local cache result")
        identities = set()
        for reference, recipe in local.items():
            for revision, entry in recipe["revisions"].items():
                identities.add(f"{reference}#{revision}")
                for package, details in entry.get("packages", {}).items():
                    revisions = details.get("revisions", {})
                    if not revisions:
                        raise ValueError("package revision missing; request all revisions")
                    for package_revision in revisions:
                        identities.add(f"{reference}#{revision}:{package}#{package_revision}")
        if not identities or not any(value.startswith("llvm/23.1.2@veyr/m1#") and ":" in value
                                     for value in identities):
            raise ValueError("prepared LLVM package identity missing")
        return identities
    except (KeyError, TypeError, AttributeError, ValueError) as error:
        raise BaselineError(f"Invalid Conan cache observation: {error}") from error


def validate_setup_log(path, selection, root, before):
    """Require explicit successful setup for this selection, without package work."""
    text = path.read_text(encoding="utf-8")
    match = re.search(r"^Command: (.+)$", text, re.MULTILINE)
    try:
        argv = ast.literal_eval(match[1]) if match else None
        if isinstance(argv, list) and argv[1:2] == ["-B"]:
            argv = [argv[0], *argv[2:]]
        if not isinstance(argv, list) or argv[1:3] != [str(root / "scripts/dev.py"), "setup"]:
            raise ValueError("expected recorded developer setup command")
        if "setup" not in argv or "--tools-only" in argv or "--init-lock" in argv:
            raise ValueError("expected dependency setup preserving existing locks")
        for flag, expected in (("--profile", selection.profile), ("--config", selection.config),
                               ("--environment", selection.environment),
                               ("--conan-home", str(selection.conan_home))):
            if argv[argv.index(flag) + 1] != expected:
                raise ValueError(f"setup {flag} differs from selected input")
        if f"Working directory: {root}\n" not in text or not re.search(
                r"\nExit: 0; elapsed: [\d.]+s\s*$", text):
            raise ValueError("missing successful final setup result/cwd")
        if before.stat().st_mtime_ns > path.stat().st_mtime_ns:
            raise ValueError("cache snapshot must precede setup log")
    except (ValueError, IndexError, SyntaxError) as error:
        raise BaselineError(f"Invalid warm-setup evidence {path}: {error}") from error
    if re.search(r"Building from source|Downloading|^(?!.*\(test package\)).*: Calling build\(\)", text, re.MULTILINE):
        raise BaselineError("Setup performed dependency download/build; warm reuse not established")
    for reference in ("llvm/23.1.2@veyr/m1", "gtest/1.18.0"):
        if f"{reference}: Already installed!" not in text:
            raise BaselineError(f"Setup log lacks explicit cached reuse for {reference}")


class FoundationChecks:
    def __init__(self, selection, selected, baseline, *, root=ROOT, external_dir=None,
                 cache_before=None, setup_log=None):
        self.selection, self.selected, self.baseline = selection, selected, baseline
        self.root = Path(root).resolve()
        self.external_dir = Path(external_dir).resolve() if external_dir else None
        self.cache_before, self.setup_log = cache_before, setup_log
        self.workspace = None
        self.results, self.contexts = {}, {}

    def ready(self):
        entry = policy_entry(self.baseline, self.selection.profile, self.selection.environment)
        observed = collect_observations(entry, self.selection.config,
                                        self.selection.conan_home, self.selection.conan_executable)
        checks = check_prerequisites(self.baseline, self.selected, observed)
        failures = [check for check in checks if check["status"] != "ok"]
        if failures:
            raise BaselineError("Prerequisites not ready: " + "; ".join(
                f"{check['identity']} expected={check['expected']}; actual={check['actual']}"
                for check in failures))
        print(f"Read-only doctor: {len(checks)}/{len(checks)}; {self.selection.environment}", flush=True)
        self.results["doctor"] = checks

    def output(self):
        if self.workspace is None:
            parent = self.root / "build/foundation-acceptance"
            parent.mkdir(parents=True, exist_ok=True)
            self.workspace = Path(tempfile.mkdtemp(
                prefix=f"{self.selection.profile}-{self.selection.config}-", dir=parent))
            print(f"Acceptance output retained at {self.workspace}", flush=True)
        return self.workspace

    def context(self, provider=None):
        provider = provider or self.selection.provider
        if provider not in self.contexts:
            llvm = None if provider == "managed" else self.external_dir
            if provider != "managed" and (llvm is None or not (llvm / "LLVMConfig.cmake").is_file()):
                raise BaselineError(f"{provider} requires real explicit --llvm-dir; no fallback")
            selection = replace(self.selection, provider=provider, llvm_dir=llvm,
                                build_dir=self.output() / provider / self.selection.config)
            self.contexts[provider] = Operations(selection, self.selected, root=self.root)
        return self.contexts[provider]

    def prepared(self, provider=None):
        operations = self.context(provider)
        operations.configure()  # Existing Conan --no-remote --build=never path.
        operations.build()
        operations.test()
        operations.verify()  # Reuses this invocation's CTest; validates compile database.
        directory = operations.selection.build_dir
        cache = (directory / "CMakeCache.txt").read_text()
        match = re.search(r"^LLVM_DIR:PATH=(.+)$", cache, re.MULTILINE)
        if not match or not (Path(match[1]) / "LLVMConfig.cmake").is_file():
            raise BaselineError("Native configure did not retain actual LLVM configuration identity")
        if operations.selection.provider != "managed" and Path(match[1]).resolve() != self.external_dir:
            raise BaselineError("External LLVM selection changed; no fallback allowed")
        self.results[operations.selection.provider] = {
            "build_dir": str(directory), "llvm_config": match[1],
            "compile_commands": str(directory / "compile_commands.json"),
            "configure_build_ctest_verify": "passed",
        }

    def providers(self):
        if self.external_dir is None or not (self.external_dir / "LLVMConfig.cmake").is_file():
            raise BaselineError("Provider positives require real explicit --llvm-dir; no fallback")
        for provider in ("managed", "system", "custom"):
            self.prepared(provider)

    def runtime(self):
        # Native policy configure checks headers/runtime; real Support tests run
        # through CTest. Its strict rejection wrappers own temporary fixtures.
        self.prepared()
        directory = self.context().selection.build_dir
        run_command(["ctest", "--test-dir", str(directory), "--output-on-failure",
                     "--stop-on-failure", "--no-tests=error", "-R",
                     r"^(LLVMIntegration\.|LLVMPolicy\.(WrongVersion|WrongRuntime|WrongTriple))"],
                    cwd=self.root, env={**os.environ, **self.selected["environment"]})
        self.results["runtime"] = "Native Support and strict version/runtime/triple rejection tests passed"

    def cache_snapshot(self, destination):
        destination = Path(destination).absolute()
        allowed = (self.root / "build", self.root / ".veyr-dev")
        if not any(path.resolve() in destination.resolve().parents for path in allowed):
            raise BaselineError("Cache observations must remain under ignored build/ or .veyr-dev/")
        if destination.exists() or destination.is_symlink():
            raise BaselineError(f"Refusing to overwrite Conan observation {destination}")
        if not self.selection.conan_home.is_dir():
            raise BaselineError("Missing prepared Conan home; run separate explicit setup")
        destination.parent.mkdir(parents=True, exist_ok=True)
        with conan_home_guard(self.selection, self.root):
            run_command([str(self.selection.conan_executable), "list", "*#*:*#*", "--cache",
                         "--format=json", f"--out-file={destination}"], cwd=self.root,
                        env={**os.environ, **self.selected["environment"],
                             "CONAN_HOME": str(self.selection.conan_home)})
        identities = package_identities(json.loads(destination.read_text()))
        print(f"Conan-owned cache observation: {destination}; {len(identities)} identities", flush=True)
        return identities

    def offline(self):
        """One intentional missing-cache rejection; never retry/repair that home."""
        # Empty recipe/package cache is intentionally external to the checkout.
        # Retain it and failed output for inspection, rather than clean on failure.
        home = Path(tempfile.mkdtemp(prefix="veyr-foundation-empty-conan-"))
        profiles = []
        for kind, paths in (("h", self.selected["host_profiles"]), ("b", self.selected["build_profiles"])):
            for path in paths:
                profiles.extend([f"-pr:{kind}", str(path)])
        argv = [str(self.selection.conan_executable), "install", str(self.root / "conanfile.py"),
                *profiles, "-o:h", "&:llvm_provider=managed", f"--lockfile={self.selection.lockfile}",
                "--no-remote", "--build=never", f"--output-folder={self.output() / 'offline'}"]
        output = []
        original = sys.stdout

        class Tee:
            def write(self, value):
                output.append(value)
                return original.write(value)

            def flush(self):
                original.flush()

        with conan_home_guard(replace(self.selection, conan_home=home), self.root):
            try:
                with contextlib.redirect_stdout(Tee()):
                    run_command(argv, cwd=self.root, env={**os.environ, **self.selected["environment"],
                                                        "CONAN_HOME": str(home)})
            except subprocess.CalledProcessError as error:
                text = "".join(output)
                # Keep raw evidence; disable Windows newline translation when writing it.
                (self.output() / "offline-rejection.log").write_text(text, encoding="utf-8", newline="")
                diagnostic_text = text.replace("\r\n", "\n")
                missing_recipe = re.search(
                    r"^gtest/1\.18\.0: Not found in local cache, looking in remotes\.\.\.$",
                    diagnostic_text, re.MULTILINE)
                no_remote = re.search(
                    r"^ERROR: Package 'gtest/1\.18\.0' not resolved: No remote defined\. Required by ''$",
                    diagnostic_text, re.MULTILINE)
                if error.returncode != 1 or not (missing_recipe and no_remote) or re.search(
                        r"Downloading|Building from source|Calling build\(\)", text):
                    raise  # Unexpected exit/diagnostic remains a hard stop.
                self.results["offline"] = {"expected_exit": 1, "empty_home": str(home),
                                            "log": str(self.output() / "offline-rejection.log"),
                                            "coverage": "missing recipe/package cache; not a binary-only miss"}
                print("Expected isolated empty-cache rejection observed once; no acquisition/retry", flush=True)
                return
        raise BaselineError("Empty-cache offline install unexpectedly succeeded; no prepared-cache proof")

    def cache(self):
        if self.cache_before is None or self.setup_log is None:
            raise BaselineError("Cache comparison needs --cache-before and --setup-log from separately explicit setup; no setup run")
        validate_setup_log(self.setup_log, self.selection, self.root, self.cache_before)
        before = package_identities(json.loads(self.cache_before.read_text()))
        after = self.cache_snapshot(self.output() / "cache-after.json")
        if before != after:
            raise BaselineError(f"Conan package/revision identities changed: removed={sorted(before-after)}; added={sorted(after-before)}")
        self.results["cache"] = {"unchanged_conan_identities": sorted(after),
                                 "before": str(self.cache_before), "setup_log": str(self.setup_log)}

    def clean(self):
        # Only this harness's disposable selected tree; no normal-output clean.
        selection = replace(self.selection, build_dir=self.output() / "clean" / self.selection.config)
        other = selection.build_dir.with_name("Release" if selection.config == "Debug" else "Debug")
        selection.build_dir.mkdir(parents=True)
        other.mkdir()
        (selection.build_dir / "disposable.txt").write_text("selected fixture")
        (other / "preserved.txt").write_text("other configuration")
        protected = [other, self.selection.build_dir,
                     self.selection.build_dir.with_name(other.name), self.selection.conan_home,
                     self.selection.conan_executable.parent.parent]
        before = {str(path): tree_identity(path) for path in protected}
        Operations(selection, self.selected, root=self.root).clean()
        if os.path.lexists(selection.build_dir) or before != {
                str(path): tree_identity(path) for path in protected}:
            raise BaselineError("Scoped clean failed selected-output/preserved-tree contract")
        self.results["clean"] = before

    def execute(self, case):
        if case == "all-checks" and (self.cache_before is None or self.setup_log is None
                                     or self.external_dir is None):
            raise BaselineError("all-checks requires real --llvm-dir, --cache-before and --setup-log; setup remains separate")
        self.ready()
        before = input_identity(self.selection, self.selected, self.root)
        for name in (("providers", "runtime", "cache", "clean", "offline") if case == "all-checks" else (case,)):
            getattr(self, name)()
            if before != input_identity(self.selection, self.selected, self.root):
                raise BaselineError("Tracked profile/lock/baseline changed during acceptance")
        report = {"host": self.selection.host, "environment": self.selection.environment,
                  "profile": self.selection.profile, "config": self.selection.config,
                  "conan_home": str(self.selection.conan_home), "input_sha256": before,
                  "task_checks": self.results, "windows_and_ci": "independently pending",
                  "macos": "deferred", "phase_verification": "not performed"}
        destination = self.output() / "results.json"
        destination.write_text(json.dumps(report, indent=2) + "\n")
        print(f"Task-check results: {destination}", flush=True)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--profile", choices=tuple(PROFILES), required=True)
    parser.add_argument("--config", choices=("Debug", "Release"), required=True)
    parser.add_argument("--environment", choices=("linux-developer", "windows-developer", "ubuntu-ci", "windows-ci"))
    parser.add_argument("--llvm-provider", choices=("managed", "system", "custom"), default="managed")
    parser.add_argument("--llvm-dir", type=Path, help="Real explicit external config; also required for providers/all-checks")
    parser.add_argument("--conan-home", type=Path)
    parser.add_argument("--case", choices=("prepared", "providers", "runtime", "cache", "clean", "offline", "all-checks"), default="prepared")
    parser.add_argument("--cache-snapshot", type=Path, help="cache only: raw Conan list observation before separately explicit setup")
    parser.add_argument("--cache-before", type=Path)
    parser.add_argument("--setup-log", type=Path)
    args = parser.parse_args(argv)
    if args.cache_snapshot and args.case != "cache":
        parser.error("--cache-snapshot requires --case cache")
    try:
        if platform.system() != PROFILES[args.profile] or platform.machine().casefold() not in {"x86_64", "amd64"}:
            raise BaselineError("Native Windows/Linux x86_64 profile required; macOS/cross compilation deferred")
        environment = args.environment
        if os.environ.get("GITHUB_ACTIONS", "").casefold() == "true":
            expected = "ubuntu-ci" if platform.system() == "Linux" else "windows-ci"
            if environment not in (None, expected):
                raise BaselineError("CI needs independently reviewed CI identity; developer evidence cannot certify CI")
            environment = expected
        baseline = read_baseline()
        selected = select_inputs(baseline, args.profile, args.config, environment=environment)
        selection = make_selection(baseline, selected, args.llvm_provider,
                                   args.llvm_dir if args.llvm_provider != "managed" else None,
                                   args.conan_home)
        checks = FoundationChecks(selection, selected, baseline, external_dir=args.llvm_dir,
                                  cache_before=args.cache_before, setup_log=args.setup_log)
        if args.cache_snapshot:
            checks.ready()
            checks.cache_snapshot(args.cache_snapshot)
        else:
            checks.execute(args.case)
        return 0
    except subprocess.CalledProcessError as error:
        print(f"Foundation stopped: command={error.cmd!r}; exit={error.returncode}; no retry", file=sys.stderr)
        return error.returncode
    except (BaselineError, OSError, ValueError) as error:
        print(f"Foundation stopped: {error}; no retry/acquisition/fallback", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
