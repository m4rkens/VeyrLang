"""Developer commands. Setup/all may acquire; configure/build/test/verify use existing cache only."""

import argparse
from dataclasses import asdict
import json
import hashlib
import re
import os
from pathlib import Path
import platform
import subprocess
import shutil
import sys

from devtools.doctor import (
    BaselineError, CONAN_VERSION, PROFILES, check_prerequisites, collect_observations,
    conan_tool_remedy, inspect_conan_tool,
    make_selection, policy_entry, read_baseline, select_inputs,
)
from devtools.conan_ops import conan_home_guard, configure_project, setup_dependencies
from devtools.runner import run_command


def bootstrap_conan(selection, entry, runner=None):
    """Install the pinned CLI only when absent; reuse or fail, never repair/retry."""
    expected_python = entry["tools"]["python"]
    if platform.python_version() != expected_python or sys.prefix != sys.base_prefix:
        raise BaselineError(f"Setup requires reviewed base Python {expected_python}; "
                            f"actual={platform.python_version()} at {sys.executable}, "
                            f"prefix={sys.prefix}, base_prefix={sys.base_prefix}")
    executable = selection.conan_executable
    directory = executable.parent.parent
    python = executable.with_name("python.exe" if entry["os"] == "Windows" else "python")
    env = {**os.environ, "CONAN_HOME": str(selection.conan_home),
           "PIP_CONFIG_FILE": os.devnull, "PYTHONNOUSERSITE": "1"}
    env.pop("PYTHONPATH", None)
    env.pop("PYTHONHOME", None)

    def run(argv, capture=False):
        print(f"tools-only: {argv!r}", flush=True)
        try:
            if runner is None:
                result = subprocess.run(argv, cwd=directory.parents[2], env=env, check=True,
                                        stdout=subprocess.PIPE if capture else None,
                                        stderr=subprocess.STDOUT if capture else None,
                                        text=True, encoding="utf-8", errors="replace")
            else:
                result = runner(argv, cwd=directory.parents[2], env=env, capture_output=capture)
            return result.stdout.strip() if capture else None
        except (OSError, subprocess.CalledProcessError) as error:
            raise BaselineError(f"Conan CLI setup stopped: actual={error}. "
                                f"{conan_tool_remedy(executable)}") from error

    for path in (directory.parent.parent, directory.parent, directory):
        if path.is_symlink() or path.is_junction():
            raise BaselineError(f"Conan CLI setup refused redirected path: actual={path}. "
                                f"{conan_tool_remedy(executable)}")
    created = not directory.exists()
    if created:
        run([str(Path(sys.executable).absolute()), "-I", "-m", "venv", str(directory)])
        run([str(python), "-I", "-m", "pip", "--isolated", "--require-virtualenv",
             "--disable-pip-version-check", "install", "--index-url", "https://pypi.org/simple",
             "--no-cache-dir", "--only-binary=:all:", f"conan=={CONAN_VERSION}"])
    actual = inspect_conan_tool(executable, expected_python)
    if actual != CONAN_VERSION:
        raise BaselineError(f"Conan CLI version mismatch: expected={CONAN_VERSION}; actual={actual}. "
                            f"{conan_tool_remedy(executable)}")
    # Conan CLI startup can initialize/migrate its home, so launch only in setup.
    output = run([str(executable), "--version"], capture=True)
    if output != f"Conan version {CONAN_VERSION}":
        raise BaselineError(f"Conan CLI launch version mismatch: expected={CONAN_VERSION}; "
                            f"actual={output.removeprefix('Conan version ')}. "
                            f"{conan_tool_remedy(executable)}")
    run([str(python), "-I", "-m", "pip", "--isolated", "--require-virtualenv",
         "--disable-pip-version-check", "check"])
    return "created" if created else "reused"


class Operations:
    """Invocation-local composition; no package solving, persistence or acquisition policy."""

    def __init__(self, selection, selected, *, root=None, prerequisite=None,
                 setup_prerequisite=None, initialize_lock=False):
        self.selection, self.selected = selection, selected
        self.root = Path(root or selection.lockfile.parents[2]).resolve()
        self.prerequisite = prerequisite or (lambda: None)
        self.setup_prerequisite = setup_prerequisite or self.prerequisite
        self.initialize_lock, self.done = initialize_lock, set()

    def header(self, step):
        s = self.selection
        print(f"step: {step}; environment={s.environment}; host={s.host}; target={s.target}; "
              f"profile={s.profile}; build-profile={s.build_profile}; config={s.config}; "
              f"provider={s.provider}\nconan={s.conan_executable}; home={s.conan_home}; "
              f"lock={s.lockfile}; build={s.build_dir}", flush=True)

    def ready(self):
        if "prerequisites" not in self.done:
            self.prerequisite()
            self.done.add("prerequisites")

    def run(self, argv):
        env = {**os.environ, **self.selected["environment"],
               "CONAN_HOME": str(self.selection.conan_home)}
        return run_command(argv, cwd=self.root, env=env)

    def identity(self):
        """Only configure inputs/output identity; Conan retains its package graph/lock."""
        inputs = {self.root / "scripts/devtools/baseline.json", self.root / "conanfile.py",
                  self.root / "CMakeLists.txt", self.root / "CMakePresets.json",
                  self.selection.lockfile, *self.selected["host_profiles"],
                  *self.selected["build_profiles"]}
        for directory, pattern in (("cmake", "*.cmake"), ("conan/recipes", "conanfile.py")):
            inputs.update((self.root / directory).rglob(pattern))
        inputs.update(self.root.glob("tests/**/CMakeLists.txt"))
        inputs.update((self.selection.build_dir / "generators").glob("*.cmake"))
        inputs.add(self.selection.build_dir / "tests/CTestTestfile.cmake")
        inputs.update(self.selection.build_dir / name for name in
                      ("CMakeCache.txt", "build.ninja", "CTestTestfile.cmake", "compile_commands.json"))
        payload = {"selection": asdict(self.selection), "inputs": self.selected,
                   "python": str(Path(sys.executable).absolute()),
                   "files": {str(path): hashlib.sha256(path.read_bytes()).hexdigest()
                             if path.is_file() else None for path in sorted(inputs)}}
        return hashlib.sha256(json.dumps(payload, sort_keys=True, default=str).encode()).hexdigest()

    @property
    def stamp(self):
        return self.selection.build_dir / ".veyr-config-inputs.json"

    def configured(self):
        if not all((self.selection.build_dir / name).is_file() for name in
                   ("CMakeCache.txt", "build.ninja", "compile_commands.json")):
            return False
        try:
            return json.loads(self.stamp.read_text())["configure_inputs_sha256"] == self.identity()
        except (OSError, ValueError, KeyError, TypeError):
            return False

    def setup(self):
        if "setup" in self.done:
            return
        self.header("setup (explicit acquisition)")
        setup_dependencies(self.selection, self.selected, root=self.root,
                           initialize_lock=self.initialize_lock,
                           ensure_prerequisites=self.setup_prerequisite, runner=run_command)
        self.done.update(("setup", "prerequisites"))

    def configure(self):
        if "configure" in self.done:
            return
        self.ready()
        self.header("configure (cache only)")
        configure_project(self.selection, self.selected, root=self.root, runner=run_command)
        self.selection.build_dir.mkdir(parents=True, exist_ok=True)
        self.stamp.write_text(json.dumps({"configure_inputs_sha256": self.identity()}, indent=2) + "\n")
        self.done.add("configure")

    def ensure_configured(self):
        self.ready()
        if "configure" not in self.done and not self.configured():
            print("Configuration missing/stale; regenerating from existing lock/cache only.", flush=True)
            self.configure()

    def build(self):
        if "build" in self.done:
            return
        self.ensure_configured()
        self.header("build (incremental)")
        self.run(["cmake", "--build", str(self.selection.build_dir)])
        self.done.add("build")

    def ctest(self):
        if "ctest" in self.done:
            print("Reusing equivalent CTest suite, including include-policy, completed this invocation.", flush=True)
            return
        self.run(["ctest", "--test-dir", str(self.selection.build_dir),
                  "--output-on-failure", "--stop-on-failure", "--no-tests=error"])
        self.done.add("ctest")

    def test(self):
        if "test" in self.done:
            return
        self.build()
        self.header("test")
        self.ctest()
        self.done.add("test")

    def verify(self):
        if "verify" in self.done:
            return
        self.ensure_configured()
        self.header("verify (prepared outputs)")
        suffix = ".exe" if PROFILES[self.selection.profile] == "Windows" else ""
        for name in ("veyrc", "tests/foundation_tests", "tests/llvm_link_tests"):
            path = self.selection.build_dir / (name + suffix)
            if not path.is_file():
                raise BaselineError(f"Missing prepared native output {path}; run explicit build for this selection")
        registration = self.selection.build_dir / "tests/CTestTestfile.cmake"
        if not registration.is_file() or 'add_test("include-policy"' not in registration.read_text():
            raise BaselineError("Missing include-policy CTest registration; run explicit configure")
        database = self.selection.build_dir / "compile_commands.json"
        try:
            entries = json.loads(database.read_text())
            if not isinstance(entries, list) or not entries:
                raise ValueError("expected a non-empty compilation database")
            compiler = self.selected["environment"]["VEYR_CPP_COMPILER"]
            for entry in entries:
                command = entry.get("command") or " ".join(entry.get("arguments", []))
                if PROFILES[self.selection.profile] == "Windows":
                    compiler = compiler.replace("\\", "/").casefold()
                    command = command.replace("\\", "/").casefold()
                compiler_pattern = r"(?:^|[\s\"'])" + re.escape(compiler) + r"(?:[\s\"']|$)"
                standard_pattern = r'(?:^|\s)(?:-std=c\+\+23|/std:c\+\+(?:23|latest))(?:\s|$)'
                if (not entry.get("file") or Path(entry["directory"]).resolve() != self.selection.build_dir.resolve()
                        or not re.search(compiler_pattern, command) or not re.search(standard_pattern, command)):
                    raise ValueError("entry lacks selected directory/compiler/C++23 identity")
        except (OSError, ValueError, KeyError, TypeError, AttributeError) as error:
            raise BaselineError(f"Invalid compilation database {database}: {error}; run explicit configure/build") from error
        print(f"Compilation database: {len(entries)} selected C++23 entries", flush=True)
        self.ctest()
        self.done.add("verify")

    def all(self):
        self.setup()
        self.configure()
        self.build()
        self.test()
        self.verify()

    def clean(self):
        self.header("clean (selected build output only)")
        target = self.selection.build_dir.absolute()
        build_root = self.root / "build"
        if target == build_root or build_root not in target.parents:
            raise BaselineError(f"Unsafe clean destination {target}; select an output beneath {build_root}")
        for path in (target, *target.parents):
            if path.is_symlink() or path.is_junction():
                raise BaselineError(f"Refusing redirected clean destination {path}")
            if path == self.root:
                break
        resolved = target.resolve()
        protected = (self.root, build_root, self.selection.conan_home.resolve(),
                     self.selection.conan_executable.parent.parent.resolve())
        if build_root.resolve() not in resolved.parents or any(
                resolved == path or resolved in path.parents or path in resolved.parents
                for path in protected[2:]) or resolved in protected[:2]:
            raise BaselineError(f"Unsafe clean destination {resolved}; preserve checkout/tool environment/cache")
        if resolved.exists():
            shutil.rmtree(resolved)
        print(f"Selected output removed/absent: {resolved}; Conan tool environment and cache preserved", flush=True)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("command", choices=("doctor", "setup", "configure", "build", "test", "verify", "all", "clean"))
    parser.add_argument("--tools-only", action="store_true", help="Setup Conan CLI only; no package/dependency work")
    parser.add_argument("--init-lock", action="store_true", help="Explicit setup initialization of a missing Conan lock; review it afterward")
    parser.add_argument("--profile", choices=tuple(PROFILES))
    parser.add_argument("--build-profile", choices=tuple(PROFILES))
    parser.add_argument("--config", choices=("Debug", "Release"), default="Debug")
    parser.add_argument("--environment", choices=("linux-developer", "windows-developer", "ubuntu-ci", "windows-ci"))
    parser.add_argument("--llvm-provider", choices=("managed", "system", "custom"), default="managed")
    parser.add_argument("--llvm-dir", type=Path)
    parser.add_argument("--conan-home", type=Path)
    parser.add_argument("--json", action="store_true", help="Print prerequisite checks as JSON")
    args = parser.parse_args(argv)
    if args.tools_only and args.command != "setup":
        parser.error("--tools-only is valid only with setup")
    if args.init_lock and (args.command != "setup" or args.tools_only):
        parser.error("--init-lock requires dependency setup, without --tools-only")
    try:
        system = platform.system()
        if system not in {"Linux", "Windows"} or platform.machine().casefold() not in {"x86_64", "amd64"}:
            raise BaselineError("Only native Windows/Linux x86_64 is supported; macOS is deferred")
        profile = args.profile or {"Linux": "linux-x86_64-clang", "Windows": "windows-x86_64-clangcl"}[system]
        if PROFILES[profile] != system:
            raise BaselineError(f"Profile {profile} requires a native {PROFILES[profile]} host; cross compilation is deferred")
        baseline = read_baseline()
        environment = args.environment
        if os.environ.get("GITHUB_ACTIONS", "").casefold() == "true":
            ci_environment = "ubuntu-ci" if system == "Linux" else "windows-ci"
            if environment not in (None, ci_environment):
                raise BaselineError("GitHub Actions requires independent CI identity; developer policy cannot certify CI")
            environment = ci_environment
        selected = select_inputs(baseline, profile, args.config, environment=environment)
        selection = make_selection(baseline, selected, args.llvm_provider, args.llvm_dir,
                                   args.conan_home, args.build_profile)
        entry = policy_entry(baseline, profile, selection.environment)
        if args.command == "setup" and args.tools_only:
            with conan_home_guard(selection):
                state = bootstrap_conan(selection, entry)
            print(f"Conan CLI {CONAN_VERSION}: {state}; executable={selection.conan_executable}; "
                  f"conan-home={selection.conan_home}")
            print("Tools-only complete. No Conan dependency/package operation; native ABI/package readiness unverified.")
            return 0
        def require_prerequisites():
            observations = collect_observations(entry, args.config, selection.conan_home, selection.conan_executable)
            checks = check_prerequisites(baseline, selected, observations)
            failed = [check for check in checks if check["status"] != "ok"]
            if failed:
                raise BaselineError("Prerequisites not ready: " + "; ".join(
                    f"{check['identity']} expected={check['expected']}; actual={check['actual']}; "
                    f"{check['remedy']}" for check in failed))
        if args.command != "doctor":
            def ensure_prerequisites():
                state = bootstrap_conan(selection, entry, run_command)
                print(f"Conan CLI {CONAN_VERSION}: {state}; executable={selection.conan_executable}", flush=True)
                require_prerequisites()
            operations = Operations(selection, selected, prerequisite=require_prerequisites,
                                    setup_prerequisite=ensure_prerequisites,
                                    initialize_lock=args.init_lock)
            getattr(operations, args.command)()
            return 0
        observations = collect_observations(entry, args.config, selection.conan_home, selection.conan_executable)
        checks = check_prerequisites(baseline, selected, observations)
        ready = all(check["status"] == "ok" for check in checks)
        report = {
            "selection": {key: str(value) if isinstance(value, Path) else value
                          for key, value in asdict(selection).items()},
            "build_configuration": selected["build_configuration"],
            "compiler_target": selected["compiler_target"],
            "host_profiles": [str(path) for path in selected["host_profiles"]],
            "build_profiles": [str(path) for path in selected["build_profiles"]],
            "checks": checks,
            "prerequisites_ready": ready,
            "native_compatibility": "unverified",
            "package_readiness": "unverified",
            "pending_environments": [entry.get("environment", name)
                                     for name, entry in {**baseline["platforms"], **baseline.get("ci_environments", {})}.items()
                                     if entry.get("status") not in {"selected", "reviewed"}],
        }
        if args.json:
            print(json.dumps(report, indent=2))
        else:
            print(f"doctor: {selection.environment}; host={selection.host}; target={selection.target}")
            print(f"profile={selection.profile}; build-profile={selection.build_profile} (Release); config={selection.config}; provider={selection.provider}")
            print(f"conan-executable={selection.conan_executable}\nconan-home={selection.conan_home}\nbuild-dir={selection.build_dir}\nlockfile={selection.lockfile}")
            print("Paths are selected only; doctor creates no build/cache/lock state.")
            for check in checks:
                print(f"{check['status']}: {check['identity']}: expected={check['expected']}; actual={check['actual'] if check['actual'] is not None else 'missing'}")
                if check["remedy"]:
                    print(f"  {check['remedy']}")
                if check["detail"]:
                    print(f"  {check['detail']}")
            print(f"Prerequisites: {'ready' if ready else 'NOT READY'}. Native ABI/package compatibility: unverified.")
            print(f"Independently pending identities: {', '.join(report['pending_environments']) or 'none'}. macOS deferred.")
        return 0 if ready else 1
    except (subprocess.CalledProcessError, subprocess.TimeoutExpired, OSError, RuntimeError) as error:
        print(f"{args.command}: stopped: {error}. No retry or cleanup.", file=sys.stderr)
        return (error.returncode if error.returncode > 0 else 128 - error.returncode) if isinstance(error, subprocess.CalledProcessError) else 2
    except KeyboardInterrupt:
        print(f"{args.command}: owner interruption; no retry or cleanup.", file=sys.stderr)
        return 130
    except BaselineError as error:
        if args.json:
            print(json.dumps({"error": str(error), "prerequisites_ready": False,
                              "native_compatibility": "unverified", "package_readiness": "unverified"}))
        else:
            print(f"{args.command}: {error}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(main())
