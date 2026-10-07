"""Read-only prerequisite checks and native build selection; Conan owns packages."""

from dataclasses import dataclass
import hashlib
import json
import ntpath
import os
from pathlib import Path
import platform
import re
import shutil
import subprocess


ROOT = Path(__file__).resolve().parents[2]
DEFAULT_BASELINE = Path(__file__).with_name("baseline.json")
PROFILES = {"linux-x86_64-clang": "Linux", "windows-x86_64-clangcl": "Windows"}
TRIPLETS = {"Linux": "linux-x86_64-gnu", "Windows": "windows-x86_64-msvc"}
NATIVE_FIELDS = {
    "Linux": ("distribution", "distribution_version", "gcc", "stdlib_abi", "libc", "linker"),
    "Windows": ("vs_installation", "vctools", "crt", "sdk", "linker"),
}
PATH_FIELDS = {
    "Linux": ("c_compiler", "cpp_compiler", "linker", "gcc_install_dir", "stdlib_headers", "stdlib_library"),
    "Windows": ("c_compiler", "cpp_compiler", "linker", "vs_root", "vctools_root", "sdk_root"),
}
TOOLS = ("python", "conan", "cmake", "ninja", "clang")
CLANG_MAJORS = {"linux-developer": "22", "ubuntu-ci": "18",
                "windows-developer": "20", "windows-ci": "20"}
CONAN_VERSION = "2.33.0"


class BaselineError(ValueError):
    """Unsupported selection or incomplete prerequisite identity."""


@dataclass(frozen=True)
class BuildSelection:
    host: str
    target: str
    profile: str
    build_profile: str
    config: str
    provider: str
    llvm_dir: Path | None
    conan_home: Path
    conan_executable: Path
    build_dir: Path
    lockfile: Path
    environment: str


def _required(value, label):
    if not isinstance(value, str) or not value.strip() or value.strip().casefold() in {"pending", "unknown"}:
        raise BaselineError(f"Pending identity: {label}; supply native evidence and owner review")
    return value


def read_baseline(path=DEFAULT_BASELINE):
    try:
        baseline = json.loads(Path(path).read_text(encoding="utf-8"))
    except (OSError, ValueError) as error:
        raise BaselineError(f"Cannot read baseline {path}: {error}") from error
    if not isinstance(baseline, dict) or baseline.get("schema_version") != 1:
        raise BaselineError("Unsupported prerequisite-policy schema")
    for section in ("tools", "platforms", "backend"):
        if not isinstance(baseline.get(section), dict):
            raise BaselineError(f"Baseline requires {section}")
    return baseline


def expected_tools(baseline, entry):
    if not isinstance(entry.get("tools"), dict):
        raise BaselineError("Each environment requires its own tool identities")
    tools = {**baseline["tools"], **entry.get("tools", {})}
    # Git is a required utility, observed separately; its version is not an ABI pin.
    tools.pop("git", None)
    for tool in TOOLS:
        version = _required(tools.get(tool), f"tools.{tool}")
        if not re.fullmatch(r"\d+\.\d+\.\d+", version):
            raise BaselineError(f"tools.{tool} requires an exact version, got {version!r}")
    return tools


def policy_entry(baseline, profile, environment=None):
    developer = baseline["platforms"].get(profile)
    if not isinstance(developer, dict):
        raise BaselineError(f"Missing native profile policy: {profile}")
    if environment is None or environment == developer.get("environment"):
        return developer
    ci = baseline.get("ci_environments", {}).get(environment)
    if not isinstance(ci, dict) or ci.get("profile") != profile or ci.get("status") not in {"selected", "reviewed"}:
        raise BaselineError(f"Environment {environment} has no independently reviewed identity; developer evidence is not CI evidence")
    return ci


def select_inputs(baseline, profile, configuration="Debug", root=ROOT, environment=None):
    """Select candidate policy, never certify installed prerequisites or ABI."""
    if profile not in PROFILES:
        raise BaselineError(f"Unsupported native profile: {profile}")
    if configuration not in {"Debug", "Release"}:
        raise BaselineError(f"Unsupported configuration: {configuration}")
    entry = policy_entry(baseline, profile, environment)
    if not isinstance(entry, dict) or entry.get("status") not in {"selected", "reviewed"}:
        raise BaselineError(f"Baseline {profile} is not owner-selected; native evidence is pending")
    _required(entry.get("environment"), f"{profile}.environment")
    if environment is not None and environment != entry["environment"]:
        raise BaselineError(f"Selected policy does not identify environment {environment}")
    system = PROFILES[profile]
    if entry.get("os") != system or entry.get("arch") != "x86_64":
        raise BaselineError(f"Profile {profile} requires {system} x86_64")
    tools = expected_tools(baseline, entry)
    if baseline["backend"].get("llvm") != "23.1.2":
        raise BaselineError("Backend LLVM must remain 23.1.2")
    _required(entry.get("target"), f"{profile}.target")
    native, paths = entry.get("native"), entry.get("paths")
    if not isinstance(native, dict) or not isinstance(paths, dict):
        raise BaselineError(f"{profile} requires native identities and explicit tool paths")
    for section, fields, values in (("native", NATIVE_FIELDS[system], native), ("paths", PATH_FIELDS[system], paths)):
        for field in fields:
            _required(values.get(field), f"{profile}.{section}.{field}")
    if system == "Linux" and native["stdlib_abi"] != "libstdc++11":
        raise BaselineError("Linux profile requires libstdc++11")
    required_major = CLANG_MAJORS.get(entry["environment"])
    if tools["clang"].split(".")[0] != required_major:
        raise BaselineError(f"{system} profile requires Clang major {required_major}; profile changes need owner review")
    env = {
        "VEYR_PROFILE_OS": system,
        "VEYR_C_COMPILER": paths["c_compiler"],
        "VEYR_CPP_COMPILER": paths["cpp_compiler"],
        "VEYR_LINKER": paths["linker"],
        "VEYR_CLANG_MAJOR": required_major,
    }
    if system == "Linux":
        env["VEYR_GCC_INSTALL_DIR"] = paths["gcc_install_dir"]
    else:
        env.update({
            "VEYR_VS_ROOT": paths["vs_root"],
            "VEYR_VCTOOLS_ROOT": paths["vctools_root"],
            "VEYR_VCTOOLS_VERSION": native["vctools"],
            "VEYR_WINDOWS_SDK_DIR": paths["sdk_root"],
            "VEYR_WINDOWS_SDK_VERSION": native["sdk"],
        })
    directory = Path(root) / "conan" / "profiles"
    return {
        "host": TRIPLETS[system], "target": TRIPLETS[system],
        "compiler_target": entry["target"], "profile": profile,
        "configuration": configuration, "environment_name": entry["environment"],
        "host_profiles": (directory / profile, directory / configuration.lower()),
        "build_profiles": (directory / profile, directory / "release"),
        "build_configuration": "Release",
        "runtime_type": configuration if system == "Windows" else None,
        "build_runtime_type": "Release" if system == "Windows" else None,
        "environment": env,
    }


def make_selection(baseline, selected, provider="managed", llvm_dir=None,
                   conan_home=None, build_profile=None, root=ROOT):
    """Compute paths only: no directories, locks, package/cache state are created."""
    profile, config = selected["profile"], selected["configuration"]
    if build_profile not in (None, profile):
        raise BaselineError("Only the selected native Release build profile is supported; cross compilation is deferred")
    if provider not in {"managed", "system", "custom"}:
        raise BaselineError(f"Unsupported LLVM provider: {provider}")
    llvm_dir = Path(llvm_dir).expanduser().resolve() if llvm_dir else None
    if provider == "managed" and llvm_dir is not None:
        raise BaselineError("--llvm-dir requires explicit system or custom provider")
    if provider != "managed" and (llvm_dir is None or not (llvm_dir / "LLVMConfig.cmake").is_file()):
        raise BaselineError(f"{provider} provider requires --llvm-dir containing LLVMConfig.cmake; no fallback")
    root = Path(root).resolve()
    common = Path(_probe(["git", "rev-parse", "--git-common-dir"], cwd=root))
    common = (root / common).resolve() if not common.is_absolute() else common.resolve()
    project_key = hashlib.sha256(str(common).encode()).hexdigest()[:12]
    if conan_home is None:
        if PROFILES[profile] == "Windows":
            base = _required(os.environ.get("LOCALAPPDATA"), "LOCALAPPDATA")
            conan_home = Path(base) / "Veyr" / "conan" / project_key
        else:
            base = os.environ.get("XDG_CACHE_HOME")
            conan_home = (Path(base) if base else Path.home() / ".cache") / "veyr" / "conan" / project_key
    conan_home = Path(conan_home).expanduser().resolve()
    if conan_home == root or root in conan_home.parents:
        raise BaselineError("--conan-home must be outside the checkout")
    if conan_home == (Path.home() / ".conan2").resolve():
        raise BaselineError("The default shared .conan2 home is forbidden; select a dedicated project home")
    entry = policy_entry(baseline, profile, selected["environment_name"])
    if expected_tools(baseline, entry)["conan"] != CONAN_VERSION:
        raise BaselineError(f"Project tool bootstrap requires Conan {CONAN_VERSION}; review policy changes first")
    _, conan_executable = conan_tool_paths(entry["os"], root)
    # Observation metadata (including Git and runner image history) is not ABI identity.
    identity = hashlib.sha256(json.dumps(
        {"tools": expected_tools(baseline, entry), "os": entry["os"], "arch": entry["arch"],
         "target": entry["target"], "native": entry["native"], "paths": entry["paths"],
         "llvm_dir": str(llvm_dir)},
        sort_keys=True).encode()).hexdigest()[:12]
    build_dir = root / "build" / selected["target"] / provider / f"{profile}-{selected['environment_name']}-{identity}" / config
    return BuildSelection(selected["host"], selected["target"], profile, profile, config,
                          provider, llvm_dir, conan_home, conan_executable, build_dir,
                          root / "conan" / "locks" / f"{profile}-{config}.lock",
                          selected["environment_name"])


def _probe(argv, cwd=ROOT, env=None):
    """One read-only identity command; no shell, retry, installation or timeout."""
    try:
        result = subprocess.run(argv, cwd=cwd, stdout=subprocess.PIPE,
                                stderr=subprocess.STDOUT, text=True, encoding="utf-8",
                                errors="replace", check=True, env=env)
    except (OSError, subprocess.CalledProcessError) as error:
        raise BaselineError(f"Identity command failed {argv!r}: {error}") from error
    return result.stdout.strip()


def _path_identity(value, system):
    if value is None:
        return None
    return ntpath.normcase(ntpath.normpath(value)) if system == "Windows" else str(Path(value).resolve())


def conan_tool_paths(system, root=ROOT):
    """Project-owned CLI paths, independent of PATH and the Conan package home."""
    directory = Path(root).absolute() / ".veyr-dev" / "tools" / f"conan-{CONAN_VERSION}"
    if system == "Windows":
        return directory / "Scripts" / "python.exe", directory / "Scripts" / "conan.exe"
    if system == "Linux":
        return directory / "bin" / "python", directory / "bin" / "conan"
    raise BaselineError("Conan CLI bootstrap supports Windows/Linux only; macOS deferred")


def conan_tool_remedy(executable):
    directory = Path(executable).parent.parent
    return (f"Expected isolated Conan {CONAN_VERSION} at {executable}. If missing, run "
            "python scripts/dev.py setup --tools-only. If invalid, remove only the exact "
            f"tool environment {directory} after owner review, then rerun that setup command. "
            "Do not remove the Conan package home/cache; no automatic repair.")


def inspect_conan_tool(executable, python_version):
    """Read isolated distribution metadata; never launch Conan or touch its home."""
    executable = Path(executable)
    directory = executable.parent.parent
    python = executable.with_name("python.exe" if executable.suffix == ".exe" else "python")
    remedy = conan_tool_remedy(executable)
    try:
        if not executable.is_absolute():
            raise BaselineError("absolute project executable required")
        for path in (directory.parent.parent, directory.parent, directory, executable.parent, executable):
            if path.is_symlink() or path.is_junction():
                raise BaselineError(f"redirected tool path: {path}")
        if not directory.is_dir():
            raise BaselineError("tool environment missing")
        config = dict(line.split("=", 1) for line in
                      (directory / "pyvenv.cfg").read_text(encoding="utf-8").splitlines() if "=" in line)
        config = {key.strip(): value.strip() for key, value in config.items()}
        if config.get("include-system-site-packages", "").casefold() != "false":
            raise BaselineError("venv isolation missing: include-system-site-packages must be false")
        if not python.is_file() or not executable.is_file():
            raise BaselineError("venv Python or Conan executable missing")
        env = {**os.environ, "PYTHONNOUSERSITE": "1",
               "PYTHONDONTWRITEBYTECODE": "1"}
        env.pop("CONAN_HOME", None)
        env.pop("PYTHONPATH", None)
        env.pop("PYTHONHOME", None)
        runtime = json.loads(_probe([str(python), "-I", "-B", "-c",
                                    "import json, platform, sys; "
                                    "from importlib.metadata import distribution; "
                                    "package = distribution('conan'); print(json.dumps({"
                                    "'version': platform.python_version(), 'prefix': sys.prefix, "
                                    "'base_prefix': sys.base_prefix, 'conan_version': package.version, "
                                    "'conan_location': str(package.locate_file(''))}))"], env=env))
        if (runtime.get("version") != python_version or
                Path(runtime.get("prefix", "")).resolve() != directory.resolve() or
                runtime.get("prefix") == runtime.get("base_prefix")):
            raise BaselineError(f"expected isolated Python {python_version} at {directory}; actual={runtime}")
        location = Path(runtime.get("conan_location", "")).resolve()
        if directory.resolve() not in location.parents:
            raise BaselineError(f"Conan distribution must be inside {directory}; actual={location}")
        version = runtime.get("conan_version")
        if not isinstance(version, str) or not re.fullmatch(r"\d+\.\d+\.\d+", version):
            raise BaselineError(f"invalid Conan version metadata: actual={version!r}")
        return version
    except (OSError, ValueError, TypeError, AttributeError) as error:
        raise BaselineError(f"Conan tool invalid/missing: actual={error}. {remedy}") from error


def collect_observations(entry, configuration="Debug", conan_home=None, conan_executable=None):
    """Observe the selected native environment only, without compiling/linking."""
    system, paths = entry["os"], entry["paths"]
    observed = {"tools.python": platform.python_version()}
    errors = {}
    if conan_executable is None:
        _, conan_executable = conan_tool_paths(system)
    observed["conan_executable"] = str(conan_executable)
    try:
        observed["tools.conan"] = inspect_conan_tool(conan_executable, entry["tools"]["python"])
    except BaselineError as error:
        observed["tools.conan"] = None
        errors["tools.conan"] = str(error)

    def probe(label, argv, pattern=None, env=None):
        try:
            output = _probe(argv, env=env)
            if pattern is not None:
                match = re.search(pattern, output)
                if not match:
                    raise BaselineError(f"Unrecognized identity output for {label}: {output[:200]}")
                output = match.group(1)
            observed[label] = output
        except BaselineError as error:
            observed[label] = None
            errors[label] = str(error)

    for name, pattern in (
        ("cmake", r"cmake version (\d+\.\d+\.\d+)"),
        ("ninja", r"^(\d+\.\d+\.\d+)$"),
        ("git", r"git version (\d+\.\d+\.\d+(?:\.windows\.\d+)?)"),
    ):
        executable = shutil.which(name)
        if name == "git":
            observed["git_path"] = executable
        if executable:
            probe(f"tools.{name}", [executable, "--version"], pattern)
        else:
            observed[f"tools.{name}"] = None
    probe("tools.clang", [paths["c_compiler"], "--version"], r"clang version (\d+\.\d+\.\d+)")
    probe("tools.clang_cpp", [paths["cpp_compiler"], "--version"], r"clang version (\d+\.\d+\.\d+)")
    probe("compiler_target", [paths["c_compiler"], "-dumpmachine"])
    for field, value in paths.items():
        path = Path(value)
        is_directory = field in {"gcc_install_dir", "stdlib_headers", "vs_root", "vctools_root", "sdk_root"}
        exists = path.is_dir() if is_directory else path.is_file()
        observed[f"paths.{field}"] = str(path) if exists else None
    if system == "Linux":
        try:
            release = platform.freedesktop_os_release()
            observed["native.distribution"] = release.get("NAME")
            observed["native.distribution_version"] = release.get("VERSION_ID")
        except OSError as error:
            errors["native.distribution"] = str(error)
        probe("native.gcc", ["gcc", "-dumpfullversion"], r"^(\d+\.\d+\.\d+)$")
        probe("native.gxx", ["g++", "-dumpfullversion"], r"^(\d+\.\d+\.\d+)$")
        probe("native.libc", ["ldd", "--version"], r"^ldd \((?:GNU libc|Ubuntu GLIBC [^)\r\n]+)\) (\d+\.\d+)")
        if observed.get("native.libc"):
            observed["native.libc"] = "glibc " + observed["native.libc"]
        probe("native.linker", [paths["linker"], "--version"], r"^GNU ld \(GNU Binutils(?: for Ubuntu)?\) (\d+\.\d+(?:\.\d+)?)")
        if observed.get("native.linker"):
            observed["native.linker"] = "GNU Binutils " + observed["native.linker"]
        probe("gcc_library_file", ["gcc", "-print-libgcc-file-name"])
        observed["gcc_installation"] = str(Path(observed["gcc_library_file"]).parent) if observed.get("gcc_library_file") else None
        probe("stdlib_library", ["g++", "-print-file-name=libstdc++.so"])
    else:
        for label, variable in (("paths.vs_root", "VSINSTALLDIR"), ("paths.vctools_root", "VCToolsInstallDir"), ("paths.sdk_root", "WindowsSdkDir")):
            value = os.environ.get(variable)
            observed[label] = value if value and Path(value).is_dir() else None
        observed["native.vctools"] = os.environ.get("VCToolsVersion")
        observed["native.sdk"] = (os.environ.get("WindowsSDKVersion") or "").rstrip("\\/") or None
        installer = Path(os.environ.get("ProgramFiles(x86)", "")) / "Microsoft Visual Studio" / "Installer" / "vswhere.exe"
        try:
            installations = json.loads(_probe([str(installer), "-all", "-format", "json", "-utf8"]))
            for installation in installations:
                if _path_identity(installation.get("installationPath"), system) == _path_identity(paths["vs_root"], system):
                    observed["native.vs_installation"] = installation.get("installationVersion")
                    break
        except (BaselineError, ValueError, TypeError) as error:
            errors["native.vs_installation"] = str(error)
        probe("native.linker", [paths["linker"], "/?"], r"Version (\d+\.\d+\.\d+(?:\.\d+)?)")
        try:
            header = (Path(paths["vctools_root"]) / "include" / "crtversion.h").read_text(encoding="utf-8")
            values = [re.search(r"#define\s+_VC_CRT_" + part + r"_VERSION\s+(\d+)", header)
                      for part in ("MAJOR", "MINOR", "BUILD", "RBUILD")]
            observed["native.crt"] = ".".join(match.group(1) for match in values) if all(values) else None
        except OSError as error:
            errors["native.crt"] = str(error)
        sdk, version = Path(paths["sdk_root"]), entry["native"]["sdk"]
        vc = Path(paths["vctools_root"])
        crt_library = "msvcrtd.lib" if configuration == "Debug" else "msvcrt.lib"
        required = (sdk / "Include" / version / "um" / "Windows.h",
                    sdk / "Include" / version / "ucrt" / "corecrt.h",
                    sdk / "Lib" / version / "um" / "x64" / "kernel32.lib",
                    sdk / "Lib" / version / "ucrt" / "x64" / ("ucrtd.lib" if configuration == "Debug" else "ucrt.lib"),
                    vc / "lib" / "x64" / crt_library)
        observed["native.sdk_runtime_files"] = "present" if all(path.is_file() for path in required) else None
    observed["errors"] = errors
    return observed


def check_prerequisites(baseline, selected, observed):
    """Compare tools/paths, keeping ABI and package acceptance explicitly unproven."""
    entry = policy_entry(baseline, selected["profile"], selected["environment_name"])
    expected = {f"tools.{name}": value for name, value in expected_tools(baseline, entry).items()}
    expected["tools.clang_cpp"] = expected["tools.clang"]
    expected["compiler_target"] = entry["target"]
    expected.update({f"native.{key}": value for key, value in entry["native"].items() if key != "stdlib_abi"})
    expected.update({f"paths.{key}": value for key, value in entry["paths"].items()})
    if entry["os"] == "Linux":
        expected.update({"native.gxx": entry["native"]["gcc"],
                         "gcc_installation": entry["paths"]["gcc_install_dir"],
                         "stdlib_library": entry["paths"]["stdlib_library"]})
    else:
        expected["native.sdk_runtime_files"] = "present"
    checks = []
    for label, value in expected.items():
        actual = observed.get(label)
        same = actual == value
        if label.startswith("paths.") or label in {"gcc_installation", "stdlib_library"}:
            same = _path_identity(actual, entry["os"]) == _path_identity(value, entry["os"])
        status = "ok" if same else ("missing" if actual is None else "mismatch")
        remedy = (conan_tool_remedy(observed.get("conan_executable") or conan_tool_paths(entry["os"])[1])
                  if label == "tools.conan" else
                  "Use the reviewed native toolchain/developer environment; report changes for owner review. No automatic repair.")
        checks.append({"identity": label, "expected": value, "actual": actual,
                       "status": status, "remedy": remedy if status != "ok" else "",
                       "detail": observed.get("errors", {}).get(label, "")})
    git = observed.get("tools.git")
    checks.append({"identity": "tools.git", "expected": "available; version observational",
                   "actual": git, "path": observed.get("git_path"),
                   "status": "ok" if git else "missing",
                   "remedy": "" if git else "Make Git available for repository identity queries; no exact version pin.",
                   "detail": observed.get("errors", {}).get("tools.git", "")})
    return checks
