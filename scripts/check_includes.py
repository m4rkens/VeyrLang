"""Read-only canonical include checks for Veyr-owned C/C++ trees."""

import argparse
from dataclasses import dataclass
from pathlib import Path, PurePosixPath, PureWindowsPath
import re
import sys


SOURCE_SUFFIXES = {".c", ".cc", ".cpp", ".cxx", ".h", ".hh", ".hpp", ".hxx",
                   ".ipp", ".inc", ".ixx", ".cppm", ".mpp"}
EXCLUDED = {"build", "generated", "third_party", "third-party", "thirdparty",
            "vendor", "_deps", ".veyr-dev", ".git", "__pycache__"}
TOKENS = re.compile(
    r'//[^\n]*|/\*[\s\S]*?(?:\*/|\Z)'
    r'|(?:u8|u|U|L)?R"(?P<delimiter>[^\s()\\]{0,16})\([\s\S]*?\)(?P=delimiter)"'
    r'|"(?:\\[^\n]|[^"\\\n])*"'
    r"|(?<![\w])(?:u8|u|U|L)?'(?:\\[^\n]|[^'\\\n])*'"
)
DIRECTIVE = re.compile(r"^[ \t\f\v]*#[ \t\f\v]*include\b", re.MULTILINE)
HEADER = re.compile(r'[ \t\f\v]*(?:<(?P<angle>[^>\n]+)>|"(?P<quoted>[^"\n]+)")')


@dataclass(frozen=True)
class Violation:
    path: Path
    line: int
    reason: str


def _source_files(directory):
    """Bounded roots only; never follow links into generated/external state."""
    if not directory.is_dir() or directory.is_symlink():
        return
    for parent, directories, files in directory.walk(follow_symlinks=False):
        directories[:] = sorted(name for name in directories
                                if name.casefold() not in EXCLUDED
                                and not (parent / name).is_symlink())
        for name in sorted(files):
            path = parent / name
            if path.suffix.casefold() in SOURCE_SUFFIXES and not path.is_symlink():
                yield path


def _directives(text):
    """Mask comments/literals, respecting line splicing and physical line numbers."""
    characters, lines = [], []
    index, line = 0, 1
    while index < len(text):
        if text.startswith("\\\n", index):
            index += 2
            line += 1
            continue
        characters.append(text[index])
        lines.append(line)
        line += text[index] == "\n"
        index += 1
    source = "".join(characters)
    code, uncommented = list(source), list(source)
    for token in TOKENS.finditer(source):
        comment = token.group().startswith(("//", "/*"))
        for position in range(token.start(), token.end()):
            if source[position] != "\n":
                code[position] = " " if comment else "@"
                if comment:
                    uncommented[position] = " "
    code, uncommented = "".join(code), "".join(uncommented)
    for directive in DIRECTIVE.finditer(code):
        header = HEADER.match(uncommented, directive.end())
        if header:
            yield lines[code.index("#", directive.start(), directive.end())], \
                header.group("angle") or header.group("quoted"), header.group("quoted") is not None


def _reason(spelling, quoted, inventory):
    portable = spelling.replace("\\", "/")
    parts = portable.split("/")
    if (PurePosixPath(spelling).is_absolute() or PureWindowsPath(spelling).drive
            or spelling.startswith("\\")):
        return f"absolute/rooted include path {spelling!r}; use canonical <veyr/...> for owned headers"
    if any(part in {".", ".."} for part in parts):
        return f"relative include path {spelling!r}; use canonical <veyr/...> for owned headers"
    folded = portable.casefold()
    canonical = inventory.get(folded)
    if canonical:
        if spelling != canonical:
            return f"noncanonical owned spelling {spelling!r}; expected <{canonical}>"
        if quoted:
            return f"quoted owned header {spelling!r}; expected <{canonical}>"
    elif parts[0].casefold() == "veyr":
        return f"unknown owned header {spelling!r}; use exact inventory spelling under include/veyr"
    else:
        candidates = sorted(value for value in inventory.values()
                            if folded in {value[5:].casefold(), value.rsplit("/", 1)[-1].casefold()}
                            or folded.endswith("/" + value.casefold()))
        if candidates:
            expected = " or ".join(f"<{value}>" for value in candidates)
            return f"source-directory/bare owned include {spelling!r}; expected {expected}"
    return None


def check_tree(root):
    root = Path(root).resolve()
    if not root.is_dir():
        raise ValueError(f"Repository root is not a directory: {root}")
    inventory = {}
    for path in _source_files(root / "include" / "veyr"):
        canonical = path.relative_to(root / "include").as_posix()
        folded = canonical.casefold()
        if folded in inventory:
            raise ValueError(f"Case-ambiguous owned inventory: {inventory[folded]} and {canonical}")
        inventory[folded] = canonical
    roots = [root / name for name in ("include", "src", "tests")]
    recipes = root / "conan" / "recipes"
    if recipes.is_dir() and not recipes.is_symlink():
        roots.extend(recipe / "test_package" for recipe in sorted(recipes.iterdir())
                     if recipe.is_dir() and not recipe.is_symlink())
    violations = []
    for directory in roots:
        for path in _source_files(directory):
            for line, spelling, quoted in _directives(path.read_text(encoding="utf-8-sig")):
                reason = _reason(spelling, quoted, inventory)
                if reason:
                    violations.append(Violation(path.relative_to(root), line, reason))
    return violations


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--root", required=True, type=Path)
    args = parser.parse_args(argv)
    try:
        violations = check_tree(args.root)
    except (OSError, UnicodeError, ValueError) as error:
        print(f"include-policy: {error}", file=sys.stderr)
        return 2
    for violation in violations:
        print(f"{violation.path.as_posix()}:{violation.line}: include-policy: {violation.reason}",
              file=sys.stderr)
    if violations:
        return 1
    print("include-policy: canonical owned includes passed")
    return 0


if __name__ == "__main__":
    sys.exit(main())
