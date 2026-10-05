"""Portable temporary-tree include-policy fixtures; no native/package work."""

import contextlib
import io
from pathlib import Path
import sys
import tempfile
import unittest

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "scripts"))
import check_includes


class IncludePolicyTests(unittest.TestCase):
    def setUp(self):
        temporary = tempfile.TemporaryDirectory()
        self.addCleanup(temporary.cleanup)
        self.root = Path(temporary.name) / "Checkout with spaces"
        self.write("include/veyr/build_info.hpp", "#pragma once\n")
        self.write("include/veyr/detail/span.hpp", "#pragma once\n")

    def write(self, name, text):
        path = self.root / name
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(text, encoding="utf-8")
        return path

    def check(self, text):
        self.write("src/example.cpp", text)
        return check_includes.check_tree(self.root)

    def test_canonical_and_ordinary_external_styles_pass(self):
        self.assertEqual(self.check('#include <veyr/build_info.hpp>\n'
                                    '#include <vector>\n#include "gtest/gtest.h"\n'
                                    '#include <llvm/ADT/SmallVector.h>\n'), [])

    def test_quoted_owned_headers_fail_with_location_and_remedy(self):
        failures = self.check('// comment\n#include "veyr/build_info.hpp"\n')
        self.assertEqual([(str(f.path), f.line) for f in failures], [(str(Path("src/example.cpp")), 2)])
        self.assertIn("quoted owned header", failures[0].reason)
        self.assertIn("<veyr/build_info.hpp>", failures[0].reason)

    def test_relative_paths_fail_for_both_separators_and_styles(self):
        for path in ("../include/veyr/build_info.hpp", "./veyr/build_info.hpp",
                     "veyr/detail/../build_info.hpp", r"..\include\veyr\build_info.hpp"):
            for opening, closing in (("<", ">"), ('"', '"')):
                with self.subTest(path=path, opening=opening):
                    failures = self.check(f"#include {opening}{path}{closing}\n")
                    self.assertEqual(len(failures), 1)
                    self.assertIn("relative include path", failures[0].reason)

    def test_absolute_and_windows_rooted_paths_fail_on_every_host(self):
        for path in ("/include/veyr/build_info.hpp", "C:/include/veyr/build_info.hpp",
                     r"C:\include\veyr\build_info.hpp", r"\\server\share\header.hpp",
                     r"\rooted\header.hpp", "C:header.hpp"):
            with self.subTest(path=path):
                failures = self.check(f"#include <{path}>\n")
                self.assertEqual(len(failures), 1)
                self.assertIn("absolute/rooted", failures[0].reason)

    def test_bare_and_source_directory_owned_names_fail(self):
        for path in ("build_info.hpp", "BUILD_INFO.HPP", "span.hpp", "detail/span.hpp",
                     "include/veyr/build_info.hpp"):
            with self.subTest(path=path):
                failures = self.check(f'#include "{path}"\n')
                self.assertEqual(len(failures), 1)
                self.assertIn("source-directory/bare", failures[0].reason)

    def test_wrong_case_and_backslash_owned_spelling_fail(self):
        for path in ("Veyr/build_info.hpp", "veyr/Build_Info.hpp", "veyr/DETAIL/span.hpp",
                     r"veyr\build_info.hpp"):
            with self.subTest(path=path):
                failures = self.check(f"#include <{path}>\n")
                self.assertEqual(len(failures), 1)
                self.assertIn("noncanonical owned spelling", failures[0].reason)

    def test_unknown_owned_header_fails(self):
        failures = self.check("#include <veyr/missing.hpp>\n")
        self.assertEqual(len(failures), 1)
        self.assertIn("unknown owned header", failures[0].reason)

    def test_comments_strings_characters_and_raw_literals_are_ignored(self):
        text = ('// #include "veyr/build_info.hpp"\n'
                '/*\n#include "veyr/build_info.hpp"\n*/\n'
                'const char *s = "#include \\"veyr/build_info.hpp\\"";\n'
                "char quote = '\"';\n"
                'const char *r = u8R"tag(\n#include "veyr/build_info.hpp"\n)tag";\n'
                'const char *empty = R"(\n#include "veyr/build_info.hpp"\n)";\n'
                '#include <veyr/build_info.hpp> // real\n')
        self.assertEqual(self.check(text), [])

    def test_comment_spacing_and_spliced_directive_report_physical_line(self):
        failures = self.check('/* comment */ # /* gap */ include /* gap */ "veyr/build_info.hpp"\n'
                              '#inc\\\nlude "veyr/detail/span.hpp"\n')
        self.assertEqual([f.line for f in failures], [1, 2])
        self.assertTrue(all("quoted owned header" in f.reason for f in failures))

    def test_spliced_line_comment_hides_following_physical_line(self):
        self.assertEqual(self.check('// continued \\\n#include "veyr/build_info.hpp"\n'
                                    '#include <veyr/build_info.hpp>\n'), [])

    def test_non_directive_text_is_ignored_but_conditional_includes_checked(self):
        failures = self.check('int x; #include "veyr/build_info.hpp"\n'
                              '#include_next "veyr/build_info.hpp"\n'
                              '#if 0\n#include "veyr/build_info.hpp"\n#endif\n')
        self.assertEqual([f.line for f in failures], [4])

    def test_digit_separators_do_not_hide_directives_as_character_literals(self):
        failures = self.check("int value = 1'000;\n"
                              '#include "veyr/build_info.hpp"\n'
                              "int other = 2'000;\n")
        self.assertEqual([f.line for f in failures], [2])

    def test_literal_before_hash_does_not_become_directive(self):
        self.assertEqual(self.check('"literal" #include "veyr/build_info.hpp"\n'), [])

    def test_recipe_test_package_is_first_party(self):
        self.write("conan/recipes/llvm/test_package/src/probe.cpp", '#include "veyr/build_info.hpp"\n')
        failures = check_includes.check_tree(self.root)
        self.assertEqual(len(failures), 1)
        self.assertEqual(failures[0].path.as_posix(), "conan/recipes/llvm/test_package/src/probe.cpp")

    def test_generated_cache_third_party_and_unrelated_trees_are_excluded(self):
        bad = '#include "veyr/build_info.hpp"\n'
        for name in ("build/main.cpp", ".veyr-dev/main.cpp", "docs/main.cpp",
                     "third_party/main.cpp", "src/generated/main.cpp", "tests/vendor/main.cpp",
                     "tests/build/main.cpp", "include/third_party/main.hpp",
                     "conan/recipes/llvm/test_package/build/main.cpp"):
            self.write(name, bad)
        self.assertEqual(check_includes.check_tree(self.root), [])

    def test_absent_optional_trees_and_spaced_root_pass(self):
        self.assertIn(" ", str(self.root))
        self.assertEqual(check_includes.check_tree(self.root), [])

    def test_all_source_roots_are_checked_and_all_locations_reported(self):
        for name in ("include/veyr/other.hpp", "src/core/example.cpp", "tests/native/example.cpp"):
            self.write(name, '#include "veyr/build_info.hpp"\n#include <build_info.hpp>\n')
        failures = check_includes.check_tree(self.root)
        self.assertEqual(len(failures), 6)
        self.assertEqual([f.line for f in failures], [1, 2, 1, 2, 1, 2])

    def test_cli_reports_nonzero_diagnostics_and_success(self):
        self.check('#include "veyr/build_info.hpp"\n')
        errors = io.StringIO()
        with contextlib.redirect_stderr(errors):
            self.assertEqual(check_includes.main(["--root", str(self.root)]), 1)
        self.assertIn("src/example.cpp:1: include-policy:", errors.getvalue())
        self.check("#include <veyr/build_info.hpp>\n")
        output = io.StringIO()
        with contextlib.redirect_stdout(output):
            self.assertEqual(check_includes.main(["--root", str(self.root)]), 0)
        self.assertIn("passed", output.getvalue())

    def test_missing_root_fails_actionably(self):
        errors = io.StringIO()
        with contextlib.redirect_stderr(errors):
            self.assertEqual(check_includes.main(["--root", str(self.root / "missing")]), 2)
        self.assertIn("not a directory", errors.getvalue())

    def test_checker_preserves_tree_contents_and_mtimes(self):
        self.write("src/example.cpp", "#include <veyr/build_info.hpp>\n")
        def snapshot():
            return {path.relative_to(self.root): (path.read_bytes(), path.stat().st_mtime_ns)
                    for path in self.root.rglob("*") if path.is_file()}
        before = snapshot()
        self.assertEqual(check_includes.check_tree(self.root), [])
        self.assertEqual(snapshot(), before)


if __name__ == "__main__":
    unittest.main()
