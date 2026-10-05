"""Thin explicit setup and cache-only preparation; Conan owns all package mechanics."""

from contextlib import contextmanager
import os
from pathlib import Path
import subprocess
import sys

from .doctor import BaselineError, ROOT, conan_tool_paths, PROFILES


def resolve_conan_home(selection, root=ROOT):
    """Validate the existing BuildSelection, without a second identity scheme."""
    home, root = selection.conan_home.resolve(), Path(root).resolve()
    if home == root or root in home.parents:
        raise BaselineError("Conan home must be outside the checkout")
    if home == (Path.home() / ".conan2").resolve():
        raise BaselineError("Select a dedicated Conan home, not the default shared .conan2")
    if home.exists() and not home.is_dir():
        raise BaselineError(f"Conan home is not a directory: {home}")
    return home


@contextmanager
def conan_home_guard(selection, root=ROOT):
    """One small exclusive marker beside the home; no Conan cache layout changes."""
    home = resolve_conan_home(selection, root)
    marker = home.with_name(home.name + ".veyr-operation.lock")
    try:
        marker.parent.mkdir(parents=True, exist_ok=True)
        descriptor = os.open(marker, os.O_CREAT | os.O_EXCL | os.O_WRONLY, 0o600)
    except FileExistsError as error:
        raise BaselineError(f"Conflicting Conan operation for {home}; marker={marker}. "
                            "Wait for the active command. If a crashed command left this marker, "
                            "verify no operation is active, then explicitly remove only the marker; "
                            "no automatic retry or cache cleanup.") from error
    except OSError as error:
        raise BaselineError(f"Cannot guard Conan home {home}: {error}") from error
    try:
        with os.fdopen(descriptor, "w", encoding="utf-8") as handle:
            handle.write(f"pid={os.getpid()}\nhome={home}\n")
        yield home
    finally:
        marker.unlink()


def _inputs(selection, selected, root):
    root = Path(root).resolve()
    if (selection.profile != selected["profile"] or selection.config != selected["configuration"]
            or selection.environment != selected["environment_name"]
            or selection.build_profile != selection.profile):
        raise BaselineError("Conan inputs do not match the reviewed native BuildSelection")
    _, expected = conan_tool_paths(PROFILES[selection.profile], root)
    if selection.conan_executable != expected or not expected.is_file():
        raise BaselineError(f"Expected project-owned Conan executable {expected}; "
                            "run explicit setup --tools-only")
    recipe = root / "conanfile.py"
    if not recipe.is_file():
        raise BaselineError(f"Missing compiler dependency recipe {recipe}")
    profiles = []
    for kind, paths in (("h", selected["host_profiles"]), ("b", selected["build_profiles"])):
        for path in paths:
            if not Path(path).is_file():
                raise BaselineError(f"Missing reviewed profile {path}; restore tracked inputs")
            profiles.extend([f"-pr:{kind}", str(path)])
    expected_lock = root / "conan" / "locks" / f"{selection.profile}-{selection.config}.lock"
    if selection.lockfile != expected_lock:
        raise BaselineError(f"Unexpected lock selection {selection.lockfile}; expected {expected_lock}")
    build_dir = selection.build_dir.resolve()
    build_root = root / "build"
    if build_root not in build_dir.parents:
        raise BaselineError(f"Conan output must be a selected directory under {build_root}")
    if selection.provider not in {"managed", "system", "custom"}:
        raise BaselineError(f"Unsupported LLVM provider {selection.provider}")
    if selection.provider != "managed" and (
            selection.llvm_dir is None or not (selection.llvm_dir / "LLVMConfig.cmake").is_file()):
        raise BaselineError("External LLVM requires an explicit existing LLVMConfig.cmake; no fallback")
    env = {**os.environ, **selected["environment"],
           "CONAN_HOME": str(resolve_conan_home(selection, root))}
    env.pop("PYTHONPATH", None)
    env.pop("PYTHONHOME", None)
    print(f"Conan selection: environment={selection.environment}; profile={selection.profile}; "
          f"config={selection.config}; provider={selection.provider}\n"
          f"executable={expected}\nhome={env['CONAN_HOME']}\n"
          f"lockfile={selection.lockfile}\nbuild-dir={selection.build_dir}", flush=True)
    return recipe, profiles, env


def _run(argv, root, env, runner=None):
    print(f"command: {argv!r}", flush=True)
    try:
        if runner is None:
            subprocess.run(argv, cwd=root, env=env, check=True)
        else:
            runner(argv, cwd=root, env=env)
    except subprocess.CalledProcessError as error:
        raise BaselineError(f"Command stopped (exit={error.returncode}): {argv!r}. "
                            "No retry or acquisition fallback. For missing dependencies, "
                            "run explicit setup for this selection.") from error
    except OSError as error:
        raise BaselineError(f"Cannot execute {argv!r}: {error}. No retry.") from error


def setup_dependencies(selection, selected, root=ROOT, *, initialize_lock=False,
                       ensure_prerequisites=None, runner=None):
    """Explicit acquiring setup; optional prerequisite callback runs inside the guard."""
    root = Path(root).resolve()
    with conan_home_guard(selection, root):
        if ensure_prerequisites is not None:
            ensure_prerequisites()
        recipe, profiles, env = _inputs(selection, selected, root)
        executable = str(selection.conan_executable)
        options = ["-o:h", f"&:llvm_provider={selection.provider}"]
        if not selection.lockfile.is_file() and not initialize_lock:
            raise BaselineError(f"Missing Conan lock {selection.lockfile}; run explicit setup "
                                "with --init-lock, review the generated lock, then retain it. "
                                "Existing locks are never updated automatically.")
        if selection.provider == "managed" or (initialize_lock and not selection.lockfile.is_file()):
            local_recipe = root / "conan" / "recipes" / "llvm"
            if not (local_recipe / "conanfile.py").is_file():
                raise BaselineError(f"Missing reviewed LLVM recipe {local_recipe}")
            _run([executable, "export", str(local_recipe), "--user=veyr", "--channel=m1"], root, env, runner)
        if not selection.lockfile.is_file():
            # Initialize a managed superset lock for the native configuration.
            _run([executable, "lock", "create", str(recipe), *profiles,
                  "-o:h", "&:llvm_provider=managed", "--lockfile=",
                  f"--lockfile-out={selection.lockfile}"], root, env, runner)
            if not selection.lockfile.is_file():
                raise BaselineError(f"Conan did not create {selection.lockfile}; stopped without retry")
        lock = [f"--lockfile={selection.lockfile}"]
        _run([executable, "install", str(recipe), *profiles, *options, *lock,
              "--build=missing", f"--output-folder={selection.build_dir}"], root, env, runner)
        if selection.provider == "managed":
            _run([executable, "test", str(root / "conan" / "recipes" / "llvm" / "test_package"),
                  "llvm/23.1.2@veyr/m1", *profiles, *lock, "--no-remote", "--build=never"], root, env, runner)


def prepare_dependencies(selection, selected, root=ROOT, *, runner=None):
    """Generate from an existing lock/cache only; never bootstrap or acquire."""
    root = Path(root).resolve()
    if not resolve_conan_home(selection, root).is_dir():
        raise BaselineError("Missing prepared Conan home; run explicit setup for this selection")
    if not selection.lockfile.is_file():
        raise BaselineError(f"Missing Conan lock {selection.lockfile}; run explicit setup --init-lock")
    with conan_home_guard(selection, root):
        recipe, profiles, env = _inputs(selection, selected, root)
        _run([str(selection.conan_executable), "install", str(recipe), *profiles,
              "-o:h", f"&:llvm_provider={selection.provider}", f"--lockfile={selection.lockfile}",
              "--no-remote", "--build=never", f"--output-folder={selection.build_dir}"], root, env, runner)


def configure_project(selection, selected, root=ROOT, python=None, *, runner=None):
    """Prepare, then invoke the native preset once Plan 01-04 supplies its inputs."""
    root = Path(root).resolve()
    if not (root / "CMakeLists.txt").is_file() or not (root / "CMakePresets.json").is_file():
        raise BaselineError("CMake project/presets are not available until Plan 01-04; "
                            "no Conan preparation or CMake command started")
    prepare_dependencies(selection, selected, root, runner=runner)
    env = {**os.environ, **selected["environment"],
           "VEYR_BUILD_DIR": str(selection.build_dir),
           "VEYR_CONAN_TOOLCHAIN": str(selection.build_dir / "generators" / "conan_toolchain.cmake"),
           "VEYR_PYTHON": str(Path(python or sys.executable).absolute()),
           "VEYR_LLVM_PROVIDER": selection.provider,
           "VEYR_LLVM_DIR": str(selection.llvm_dir) if selection.llvm_dir else ""}
    print(f"CMake paths: build={env['VEYR_BUILD_DIR']}; "
          f"toolchain={env['VEYR_CONAN_TOOLCHAIN']}; python={env['VEYR_PYTHON']}", flush=True)
    _run(["cmake", "--preset", f"{selection.profile}-{selection.config}",
          f"-DVEYR_LLVM_PROVIDER={selection.provider}",
          f"-DVEYR_LLVM_DIR={env['VEYR_LLVM_DIR']}"], root, env, runner)
