"""Snapshot decisions against disposable export-scope trees, never real packages."""
import copy
import contextlib
import importlib.util
import io
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import tarfile
import tempfile
import unittest
from unittest.mock import patch

SCRIPT = Path(__file__).resolve().parents[2] / 'scripts/ci_conan_snapshot.py'
spec = importlib.util.spec_from_file_location('snapshot', SCRIPT)
snapshot = importlib.util.module_from_spec(spec)
spec.loader.exec_module(snapshot)

RREF = 'llvm/23.1.2@veyr/m1#' + 'a' * 32
PREF = RREF + ':' + 'b' * 40 + '#' + 'c' * 32


class SnapshotTests(unittest.TestCase):
    def setUp(self):
        self.tmp = tempfile.TemporaryDirectory()
        self.addCleanup(self.tmp.cleanup)
        self.root = Path(self.tmp.name)
        self.home = self.root / 'home'
        self.home.mkdir()
        self.archive = self.root / 'dependencies.tgz'
        self.archive.write_bytes(b'opaque Conan archive')
        self.folders = {}
        for ref, roles in ((RREF, ('export', 'export_source', 'source', 'metadata')),
                           (PREF, ('package', 'metadata'))):
            for role in roles:
                path = self.home / ('recipe' if ref == RREF else 'binary') / role
                path.mkdir(parents=True)
                (path / 'value').write_bytes(b'exact material content')
                self.folders[ref, role] = path
        self.calls = []
        self.raw = {'Local Cache': {'llvm/23.1.2@veyr/m1': {'revisions': {
            'a' * 32: {'timestamp': 1, 'packages': {'b' * 40: {
                'info': {'settings': {'build_type': 'Debug'}},
                'revisions': {'c' * 32: {'timestamp': 2}}}}}}}}}
        self.binding = {'prefix': 'compatible-', 'matched': 'compatible-old',
                        'environment': 'ubuntu-ci', 'profile': 'linux-x86_64-clang', 'config': 'Debug'}

    def conan(self, argv):
        self.calls.append(argv)
        if argv == ['--version']:
            return 'Conan version 2.33.0\n'
        if argv[0] == 'list':
            self.assertEqual(argv, ['list', '*#*:*#*', '--cache', '--format=json'])
            return json.dumps(self.raw)
        self.assertEqual(argv[:2], ['cache', 'path'])
        ref = argv[2]
        role = argv[3].split('=', 1)[1] if argv[3].startswith('--folder=') else (
            'package' if ':' in ref else 'export')
        path = self.folders[ref, role]
        if path is None:
            label = {'export_source': 'export_sources'}.get(role, role)
            display = ref.split('#', 1)[0]
            if ':' in ref:
                display += ':' + ref.split(':', 1)[1].split('#', 1)[0]
            raise subprocess.CalledProcessError(1, argv, stderr=
                f"ERROR: '{label}' folder does not exist for the reference {display}\n")
        return json.dumps({'cache_path': str(path)})

    def capture(self):
        return snapshot.observe(self.conan, self.home, self.archive, self.binding,
                                immutable=self.immutable)

    def immutable(self, refs):
        return {ref: str(self.folders[ref, 'package']) for ref in refs}

    def add_binary(self, revision='c', package_id='b', recipe_revision='a'):
        rref = 'llvm/23.1.2@veyr/m1#' + recipe_revision * 32
        pref = rref + ':' + package_id * 40 + '#' + revision * 32
        revisions = self.raw['Local Cache']['llvm/23.1.2@veyr/m1']['revisions']
        data = revisions.setdefault(recipe_revision * 32, {'timestamp': 3, 'packages': {}})
        package = data['packages'].setdefault(package_id * 40, {'info': {}, 'revisions': {}})
        package['revisions'][revision * 32] = {'timestamp': 4}
        for ref, roles in ((rref, ('export', 'export_source', 'source', 'metadata')),
                           (pref, ('package', 'metadata'))):
            for role in roles:
                if (ref, role) not in self.folders:
                    folder = self.home / str(len(self.folders)) / role
                    folder.mkdir(parents=True)
                    (folder / 'value').write_text('new completed scope')
                    self.folders[ref, role] = folder
        return pref

    def test_finalized_path_is_never_used_for_package_export_observation(self):
        original = self.conan
        finalized = self.home / 'finalized'
        finalized.mkdir()
        (finalized / 'value').write_text('unexported finalized content')
        def conan(argv):
            if argv[:3] == ['cache', 'path', PREF] and '--folder=metadata' not in argv:
                return json.dumps({'cache_path': str(finalized)})
            return original(argv)
        self.conan = conan
        before = self.capture()
        self.assertEqual(before['status'], 'supported', before)
        self.assertEqual(before['scopes'][PREF + '/package'],
                         snapshot.tree_digest(self.folders[PREF, 'package'], self.home))
        (finalized / 'value').write_text('another local-only value')
        self.assertFalse(self.decide(before)['publish'])
        (self.folders[PREF, 'package'] / 'value').write_text('changed immutable bytes')
        self.assertTrue(self.decide(before)['publish'])
        self.assertFalse(any(a[:3] == ['cache', 'path', PREF] and '--folder=metadata' not in a
                             for a in self.calls))

    @unittest.skipIf(os.name == 'nt', 'POSIX permission bits; Windows behavior needs hosted evidence')
    def test_matching_finalized_copy_cannot_hide_immutable_permissions(self):
        finalized = self.home / 'same-finalized'
        shutil.copytree(self.folders[PREF, 'package'], finalized)
        original = self.conan
        def conan(argv):
            if argv[:3] == ['cache', 'path', PREF] and '--folder=metadata' not in argv:
                return json.dumps({'cache_path': str(finalized)})
            return original(argv)
        self.conan = conan
        before = self.capture()
        (self.folders[PREF, 'package'] / 'value').chmod(0o755)
        self.assertEqual(before['scopes'][PREF + '/package'], snapshot.tree_digest(finalized, self.home))
        self.assertTrue(self.decide(before)['publish'])

    def test_missing_or_unsupported_immutable_information_cannot_skip(self):
        for resolver in (None, lambda refs: {}, lambda refs: {refs[0]: None},
                         lambda refs: {refs[0]: str(self.root)},
                         lambda refs: (_ for _ in ()).throw(OSError('unavailable'))):
            with self.subTest(resolver=resolver):
                observed = snapshot.observe(self.conan, self.home, self.archive, self.binding,
                                            immutable=resolver)
                self.assertEqual(observed['status'], 'unsupported', observed)
                self.assertTrue(self.decide(observed)['publish'])
                self.assertIn('failure', observed)

    def test_adapter_contract_requires_exact_version_resolver_and_reference_set(self):
        good = {'schema': 1, 'conan': '2.33.0',
                'resolver': 'PkgCache.pkg_layout(full_pref).package()', 'paths': {PREF: '/unused'}}
        for changed in ({'conan': '2.34.0'}, {'schema': 9}, {'resolver': 'cache.path'},
                        {'paths': {}}, {'paths': {PREF: '/unused', 'unexpected': '/other'}}):
            with self.subTest(changed=changed), patch.object(subprocess, 'check_output',
                    return_value=json.dumps({**good, **changed})), self.assertRaises(snapshot.SnapshotError):
                snapshot.immutable_paths(self.root / 'bin/conan', [PREF])

    def test_old_observation_schema_cannot_authorize_a_skip(self):
        before = self.capture()
        before['schema'] = 1
        self.assertTrue(self.decide(before)['publish'])

    def test_completed_packages_and_multiple_exact_revisions_are_observed(self):
        before = self.capture()
        for kwargs in ({'package_id': 'd'}, {'revision': 'e'}, {'recipe_revision': 'f'}):
            with self.subTest(addition=kwargs):
                pref = self.add_binary(**kwargs)
                current = self.capture()
                self.assertEqual(current['status'], 'supported', current)
                self.assertIn(pref + '/package', current['scopes'])
                self.assertTrue(self.decide(before)['publish'])
                before = current
        self.assertFalse(self.decide(before)['publish'])
        # A non-latest PREV remains in cache save's wildcard selection.
        (self.folders[PREF, 'package'] / 'value').write_text('older PREV changed')
        self.assertTrue(self.decide(before)['publish'])

    def test_removed_packages_and_recipe_or_package_revisions_are_material(self):
        for removal in ('package', 'prev', 'rrev'):
            with self.subTest(removal=removal):
                self.add_binary(revision='d', package_id='e', recipe_revision='f')
                before = self.capture()
                revisions = self.raw['Local Cache']['llvm/23.1.2@veyr/m1']['revisions']
                if removal == 'rrev':
                    del revisions['f' * 32]
                elif removal == 'package':
                    del revisions['f' * 32]['packages']['e' * 40]
                else:
                    self.add_binary(revision='d')
                    before = self.capture()
                    del revisions['a' * 32]['packages']['b' * 40]['revisions']['d' * 32]
                self.assertTrue(self.decide(before)['publish'])

    def test_unsupported_context_is_bounded_and_does_not_leak_subprocess_secrets(self):
        def fail(refs):
            raise subprocess.CalledProcessError(1, ['private'], stderr=
                'token=secret /private/home/cache\x1b[31m\r\n' + 'hostile' * 1000)
        observed = snapshot.observe(self.conan, self.home, self.archive, self.binding,
                                    immutable=fail)
        self.assertEqual(observed['status'], 'unsupported')
        failure = observed['failure']
        self.assertEqual(failure['role'], 'immutable-package')
        self.assertIn('classification', failure)
        self.assertLessEqual(len(failure['detail']), 160)
        self.assertTrue(all(32 <= ord(c) < 127 for c in failure['detail']))
        self.assertNotIn('secret', json.dumps(failure))
        self.assertNotIn('/private', json.dumps(failure))

    def decide(self, before=None, **kw):
        args = dict(event='push', ref='refs/heads/main', cold=False,
                    setup=True, integrity=True, imported=True,
                    archive=self.archive, binding=self.binding,
                    before=self.capture() if before is None else before, capture=self.capture)
        args.update(kw)
        return snapshot.decide(**args)

    def test_unchanged_warm_all_four_selections_do_not_compress(self):
        for env, profile in (('ubuntu-ci', 'linux-x86_64-clang'),
                             ('windows-ci', 'windows-x86_64-clangcl')):
            for config in ('Debug', 'Release'):
                self.binding.update(environment=env, profile=profile, config=config)
                result = self.decide()
                self.assertFalse(result['publish'], result)
                self.assertEqual(result['reason'], 'validated-export-scope-unchanged')
        self.assertTrue(self.calls)
        self.assertTrue(all(a[0] in ('--version', 'list', 'cache') for a in self.calls))
        self.assertFalse(any(a[:2] == ['cache', 'save'] for a in self.calls))

    def test_each_export_scope_content_change_requires_publication(self):
        for key, path in self.folders.items():
            before = self.capture()
            (path / 'value').write_bytes((path / 'value').read_bytes() + b'x')
            with self.subTest(scope=key):
                result = self.decide(before)
                self.assertTrue(result['publish'])
                self.assertEqual(result['reason'], 'export-scope-changed')

    def test_empty_directory_add_delete_rename_and_modes_are_material(self):
        folder = self.folders[RREF, 'source']
        before = self.capture()
        (folder / 'empty').mkdir()
        self.assertTrue(self.decide(before)['publish'])
        before = self.capture()
        (folder / 'value').rename(folder / 'renamed')
        self.assertTrue(self.decide(before)['publish'])
        before = self.capture()
        (folder / 'renamed').unlink()
        self.assertTrue(self.decide(before)['publish'])
        if os.name != 'nt':
            before = self.capture()
            (folder / 'mode').write_text('x')
            before = self.capture()
            (folder / 'mode').chmod(0o755)
            self.assertTrue(self.decide(before)['publish'])

    def test_symlink_target_is_observed_without_following(self):
        link = self.folders[RREF, 'source'] / 'link'
        try:
            link.symlink_to(self.root / 'outside')
        except OSError:
            self.skipTest('symlinks unavailable on host')
        before = self.capture()
        (self.root / 'outside').write_text('not exported by link')
        self.assertFalse(self.decide(before)['publish'])
        link.unlink()
        link.symlink_to(self.root / 'elsewhere')
        self.assertTrue(self.decide(before)['publish'])

    def test_identity_and_info_changes_require_export_but_timestamps_do_not(self):
        before = self.capture()
        revision = self.raw['Local Cache']['llvm/23.1.2@veyr/m1']['revisions']['a' * 32]
        revision['timestamp'] = 99
        revision['packages']['b' * 40]['revisions']['c' * 32]['timestamp'] = 99
        self.assertFalse(self.decide(before)['publish'])
        revision['packages']['b' * 40]['info']['settings']['build_type'] = 'Release'
        self.assertTrue(self.decide(before)['publish'])
        for section in ('recipe', 'package_id', 'package_revision'):
            def changed():
                observed = self.capture()
                observed['identities'] = [section + '-new']
                return observed
            self.assertTrue(self.decide(before, capture=changed)['publish'])

    def test_optional_absence_and_new_optional_source_are_compared(self):
        source = self.folders[RREF, 'source']
        self.folders[RREF, 'source'] = None
        before = self.capture()
        self.assertEqual(before['status'], 'supported')
        self.assertFalse(self.decide(before)['publish'])
        self.folders[RREF, 'source'] = source
        self.assertTrue(self.decide(before)['publish'])

    def test_all_optional_folders_can_be_absent_without_guessing_errors(self):
        for key in ((RREF, 'source'), (RREF, 'export_source'), (RREF, 'metadata'), (PREF, 'metadata')):
            self.folders[key] = None
        before = self.capture()
        self.assertEqual(before['status'], 'supported', before)
        self.assertFalse(self.decide(before)['publish'])

    def test_uncertain_or_unsupported_inventory_always_exports(self):
        for raw in ({}, {'Local Cache': {}}, {'Local Cache': {'bad': {}}},
                    {'Local Cache': {}, 'remote': {}}, {'new_schema': True}):
            with self.subTest(raw=raw):
                self.raw = raw
                observed = self.capture()
                self.assertEqual(observed['status'], 'unsupported')
                self.assertTrue(self.decide(observed)['publish'])
        for before in ({}, {'status': 'supported'}, {'status': 'unsupported', 'reason': 'query failed'}):
            self.assertTrue(self.decide(before=before)['publish'])

    def test_path_query_failure_or_outside_home_never_proves_unchanged(self):
        observed = snapshot.observe(lambda _: (_ for _ in ()).throw(OSError('failed')),
                                    self.home, self.archive, self.binding)
        self.assertEqual(observed['status'], 'unsupported')
        self.folders[RREF, 'source'] = self.root
        self.assertEqual(self.capture()['status'], 'unsupported')

    def test_cold_miss_partial_restore_and_failed_import(self):
        self.assertEqual(self.decide(cold=True)['reason'], 'cold-build-cache-disabled')
        self.assertFalse(self.decide(cold=True)['publish'])
        self.assertTrue(self.decide(imported=False, binding={**self.binding, 'matched': ''})['publish'])
        with self.assertRaises(snapshot.SnapshotError):
            self.decide(imported=False)
        before = self.capture()
        (self.folders[PREF, 'package'] / 'new-binary').write_bytes(b'newly completed')
        self.assertTrue(self.decide(before)['publish'])

    def test_missing_or_changed_restored_archive_is_strict_failure(self):
        before = self.capture()
        self.archive.write_bytes(b'corrupt changed archive')
        with self.assertRaises(snapshot.SnapshotError):
            self.decide(before)
        self.archive.unlink()
        with self.assertRaises(snapshot.SnapshotError):
            self.decide(before)

    def test_setup_integrity_or_import_failure_never_authorizes_publication(self):
        for kw in ({'setup': False}, {'integrity': False}):
            with self.assertRaises(snapshot.SnapshotError):
                self.decide(**kw)

    def test_binding_prefix_and_version_mismatch_cannot_skip(self):
        before = self.capture()
        for field in ('prefix', 'environment', 'profile', 'config'):
            other = dict(self.binding, **{field: 'different'})
            if field == 'prefix':
                with self.assertRaises(snapshot.SnapshotError):
                    self.decide(before, binding=other)
            else:
                self.assertTrue(self.decide(before, binding=other)['publish'])
        def changed(argv):
            return 'Conan version 9.0.0' if argv == ['--version'] else self.conan(argv)
        self.assertEqual(snapshot.observe(changed, self.home, self.archive, self.binding)['status'], 'unsupported')

    def test_pr_and_other_refs_never_publish_even_if_inventory_claims_changes(self):
        for event, ref in (('pull_request', 'refs/pull/1/merge'), ('push', 'refs/heads/topic'),
                           ('pull_request_target', 'refs/heads/main'), ('workflow_run', 'refs/heads/main')):
            result = self.decide(event=event, ref=ref)
            self.assertFalse(result['publish'])
            self.assertEqual(result['reason'], 'not-trusted-main-writer')
        self.assertTrue(self.decide(event='workflow_dispatch', imported=False,
                                    binding={**self.binding, 'matched': ''})['publish'])

    def test_publication_confirmation_requires_exact_lookup_not_action_outcome(self):
        for save, lookup, hit in (('failure', 'success', 'true'), ('success', 'failure', 'true'),
                                   ('success', 'success', ''), ('success', 'success', 'false')):
            with self.subTest(save=save, lookup=lookup, hit=hit), self.assertRaises(snapshot.SnapshotError):
                snapshot.publication_result(True, save, lookup, hit)
        self.assertEqual(snapshot.publication_result(True, 'success', 'success', 'true'), 'confirmed')
        self.assertEqual(snapshot.publication_result(False, 'skipped', 'skipped', ''), 'not-requested')

    def test_same_revisions_do_not_hide_metadata_or_source_changes(self):
        before = self.capture()
        self.folders[PREF, 'metadata'].joinpath('audit.json').write_text('new metadata without new PREV')
        self.assertEqual(before['identities'], self.capture()['identities'])
        self.assertTrue(self.decide(before)['publish'])

    def test_missing_archive_digest_and_unknown_path_error_fail_conservatively(self):
        before = self.capture()
        del before['archive_sha256']
        self.assertTrue(self.decide(before)['publish'])
        real = self.conan
        def fail(argv):
            if argv[:2] == ['cache', 'path']:
                raise subprocess.CalledProcessError(1, argv, stderr='permission denied')
            return real(argv)
        self.assertEqual(snapshot.observe(fail, self.home, self.archive, self.binding)['status'], 'unsupported')

    def test_unknown_inventory_fields_and_both_final_observation_failures_export(self):
        before = self.capture()
        self.assertTrue(self.decide(before, capture=lambda: {'status': 'unsupported'})['publish'])
        self.raw['Local Cache']['llvm/23.1.2@veyr/m1']['revisions']['a' * 32]['new_field'] = 'unknown'
        self.assertEqual(self.capture()['status'], 'unsupported')
        self.assertTrue(self.decide(before)['publish'])

    def test_exact_revision_additions_are_in_inventory_without_private_layout(self):
        baseline = snapshot.inventory(self.raw)
        revisions = self.raw['Local Cache']['llvm/23.1.2@veyr/m1']['revisions']
        revisions['d' * 32] = copy.deepcopy(revisions['a' * 32])
        self.assertNotEqual(snapshot.inventory(self.raw), baseline)
        self.assertIn('llvm/23.1.2@veyr/m1#' + 'd' * 32,
                      [item['ref'] for item in snapshot.inventory(self.raw)])
        packages = revisions['a' * 32]['packages']
        packages['e' * 40] = copy.deepcopy(packages['b' * 40])
        packages['b' * 40]['revisions']['f' * 32] = {'timestamp': 9}
        refs = [item['ref'] for item in snapshot.inventory(self.raw)]
        self.assertIn(RREF + ':' + 'e' * 40 + '#' + 'c' * 32, refs)
        self.assertIn(RREF + ':' + 'b' * 40 + '#' + 'f' * 32, refs)

    def test_changing_file_during_observation_cannot_prove_unchanged(self):
        real = snapshot.digest
        target = self.folders[RREF, 'source'] / 'value'
        def change(path):
            value = real(path)
            if path == target:
                path.write_bytes(b'concurrent mutation')
            return value
        with patch.object(snapshot, 'digest', side_effect=change):
            self.assertEqual(self.capture()['status'], 'unsupported')

    def test_operational_mtime_changes_do_not_change_material_content(self):
        before = self.capture()
        path = self.folders[RREF, 'source'] / 'value'
        os.utime(path, (1, 1))
        self.assertFalse(self.decide(before)['publish'])

    def test_deep_tree_or_special_scope_uncertainty_cannot_skip(self):
        with patch.object(snapshot, 'tree_digest', side_effect=RecursionError()):
            self.assertEqual(self.capture()['status'], 'unsupported')
        self.folders[RREF, 'source'] = self.folders[RREF, 'source'] / 'value'
        self.assertEqual(self.capture()['status'], 'unsupported')

    def test_hardlink_topology_is_unsupported_rather_than_assumed_equal(self):
        source = self.folders[RREF, 'source']
        try:
            os.link(source / 'value', source / 'hardlink')
        except OSError:
            self.skipTest('hardlinks unavailable on host')
        observed = self.capture()
        self.assertEqual(observed['status'], 'unsupported')
        self.assertTrue(self.decide(observed)['publish'])

    def test_cli_observe_decide_and_report_have_auditable_outputs(self):
        old = Path.cwd()
        self.addCleanup(os.chdir, old)
        os.chdir(self.root)
        archive = Path('.veyr-dev/conan-cache/dependencies.tgz')
        archive.parent.mkdir(parents=True)
        archive.write_bytes(self.archive.read_bytes())
        output = self.root / 'github-output'
        env = {'CONAN_HOME': str(self.home), 'VEYR_CONAN_EXECUTABLE': sys.executable,
               'GITHUB_EVENT_NAME': 'push', 'GITHUB_REF': 'refs/heads/main',
               'GITHUB_OUTPUT': str(output), 'SNAPSHOT_IMPORT_RESULT': 'success',
               'SNAPSHOT_SETUP_RESULT': 'success', 'SNAPSHOT_INTEGRITY_RESULT': 'success',
               **{'SNAPSHOT_' + k.upper(): v for k, v in self.binding.items()}}
        def invoke(operation, extra=None):
            with patch.dict(os.environ, {**env, **(extra or {})}, clear=True), \
                 patch.object(sys, 'argv', [str(SCRIPT), operation]), \
                 patch.object(subprocess, 'check_output', side_effect=lambda args, **kw: self.conan(args[1:])), \
                 patch.object(snapshot, 'immutable_paths', side_effect=lambda executable, refs: self.immutable(refs)), \
                 contextlib.redirect_stdout(io.StringIO()):
                return snapshot.main()
        self.assertEqual(invoke('observe'), 0)
        self.assertEqual(invoke('decide'), 0)
        self.assertEqual(output.read_text(), 'publish=false\n')
        decision = json.loads(Path('.veyr-dev/ci/cache-decision.json').read_text())
        self.assertEqual(decision['reason'], 'validated-export-scope-unchanged')
        self.assertEqual(invoke('report'), 0)
        self.folders[PREF, 'package'].joinpath('new').write_text('completed binary')
        self.assertEqual(invoke('decide'), 0)
        self.assertTrue(output.read_text().endswith('publish=true\n'))
        self.assertEqual(invoke('report', {'SNAPSHOT_SAVE_RESULT': 'success',
                                         'SNAPSHOT_LOOKUP_RESULT': 'success', 'SNAPSHOT_EXACT_HIT': 'false'}), 1)
        self.assertEqual(json.loads(Path('.veyr-dev/ci/cache-publication.json').read_text())['status'], 'failed')
        self.assertEqual(invoke('report', {'SNAPSHOT_SAVE_RESULT': 'success',
                                         'SNAPSHOT_LOOKUP_RESULT': 'success', 'SNAPSHOT_EXACT_HIT': 'true'}), 0)
        self.assertEqual(json.loads(Path('.veyr-dev/ci/cache-publication.json').read_text())['publication'], 'confirmed')


@unittest.skipUnless(os.environ.get('VEYR_SNAPSHOT_TEST_CONAN'),
                     'set VEYR_SNAPSHOT_TEST_CONAN to the reviewed 2.33.0 executable')
class RealConanExportTests(unittest.TestCase):
    """Data-only recipe; no compiler, remotes, dependency installs or shared homes."""

    def test_real_archive_uses_immutable_trees_with_finalizers_and_multiple_revisions(self):
        with tempfile.TemporaryDirectory(prefix='snapshot-scope-') as temporary:
            root = Path(temporary)
            home, recipe = root / 'home', root / 'recipe'
            recipe.mkdir()
            profile = root / 'profile'
            profile.write_text('[settings]\nos=' + {'win32': 'Windows', 'darwin': 'Macos'}.get(sys.platform, 'Linux') +
                               '\narch=x86_64\n')
            source = '''import os
from conan import ConanFile
from conan.tools.files import copy, save
class Probe(ConanFile):
    name = "snapshot-probe"
    version = "1.0"
    package_type = "application"
    def package(self):
        save(self, os.path.join(self.package_folder, "payload.txt"), os.environ["VEYR_FIXTURE_PAYLOAD"])
    def finalize(self):
        copy(self, "*", self.immutable_package_folder, self.package_folder)
'''
            (recipe / 'conanfile.py').write_text(source)
            executable = Path(os.environ['VEYR_SNAPSHOT_TEST_CONAN'])
            self.assertTrue(executable.is_absolute() and executable.is_file())
            commands = []
            with patch.dict(os.environ, {'CONAN_HOME': str(home), 'CONAN_NON_INTERACTIVE': '1',
                                        'NO_COLOR': '1', 'PYTHONDONTWRITEBYTECODE': '1',
                                        'VEYR_FIXTURE_PAYLOAD': 'original immutable bytes'}):
                def conan(argv, expected=0):
                    run = subprocess.run([str(executable), *argv], cwd=root, capture_output=True,
                                         text=True, encoding='utf-8')
                    commands.append({'operation': argv[:2], 'exit': run.returncode})
                    if expected is None and run.returncode:
                        raise subprocess.CalledProcessError(run.returncode, argv, run.stdout, run.stderr)
                    if expected == 0:
                        self.assertEqual(run.returncode, 0, run.stderr)
                    elif expected is not None:
                        self.assertNotEqual(run.returncode, 0)
                    return run.stdout
                profiles = ['-pr:h', str(profile), '-pr:b', str(profile), '--no-remote']
                self.assertEqual(conan(['--version']).strip(), 'Conan version 2.33.0')
                conan(['export-pkg', str(recipe), *profiles])
                conan(['install', '--requires=snapshot-probe/1.0', '--build=never',
                       '-of', str(root / 'generators'), *profiles])
                archive = root / 'snapshot.tgz'
                conan(['cache', 'save', '*#*:*#*', '--file', str(archive)])
                binding = {'prefix': 'fixture-', 'matched': 'fixture-restored',
                           'environment': 'local-fixture', 'profile': 'data-only', 'config': 'Release'}
                def capture():
                    return snapshot.observe(lambda argv: conan(argv, expected=None), home, archive, binding,
                                            immutable=lambda refs: snapshot.immutable_paths(executable, refs))
                def decision(before, integrity=True):
                    return snapshot.decide(event='push', ref='refs/heads/main', cold=False,
                        setup=True, integrity=integrity, imported=True, archive=archive,
                        binding=binding, before=before, capture=capture)
                before = capture()
                self.assertEqual(before['status'], 'supported', before)
                pref = next(i['ref'] for i in before['identities'] if ':' in i['ref'])
                immutable = Path(snapshot.immutable_paths(executable, [pref])[pref])
                finalized = Path(json.loads(conan(['cache', 'path', pref, '--format=json']))['cache_path'])
                self.assertNotEqual(immutable, finalized)
                db_before = {p: p.read_bytes() for p in home.rglob('*.sqlite3')}
                self.assertFalse(decision(before)['publish'])
                self.assertEqual(db_before, {p: p.read_bytes() for p in db_before})
                (finalized / 'payload.txt').write_text('local finalized content only')
                self.assertFalse(decision(before)['publish'])
                payload = immutable / 'payload.txt'
                original, mode = payload.read_bytes(), payload.stat().st_mode & 0o777
                if os.name != 'nt':
                    payload.chmod(mode ^ 0o100)
                    conan(['cache', 'check-integrity', '*#*:*#*'])
                    self.assertTrue(decision(before)['publish'])
                    payload.chmod(mode)
                payload.write_bytes(b'corrupted immutable bytes')
                conan(['cache', 'check-integrity', '*#*:*#*'], expected=1)
                self.assertNotEqual(before['scopes'][pref + '/package'], capture()['scopes'][pref + '/package'])
                with self.assertRaises(snapshot.SnapshotError):
                    decision({'status': 'unsupported'}, integrity=False)
                payload.write_bytes(original)
                conan(['cache', 'check-integrity', '*#*:*#*'])
                os.environ['VEYR_FIXTURE_PAYLOAD'] = 'second package revision'
                conan(['export-pkg', str(recipe), *profiles])
                self.assertTrue(decision(before)['publish'])
                previous = capture()
                (recipe / 'conanfile.py').write_text(source + '\n# second recipe revision\n')
                conan(['export-pkg', str(recipe), *profiles])
                self.assertTrue(decision(previous)['publish'])
                current = capture()
                prefs = [i['ref'] for i in current['identities'] if ':' in i['ref']]
                self.assertEqual(len(prefs), 3)  # Two PREVs under one RREV, plus one new RREV.
                conan(['cache', 'save', '*#*:*#*', '--file', str(archive)])
                paths = snapshot.immutable_paths(executable, prefs)
                checked = []
                with tarfile.open(archive) as tar:
                    listing = json.load(tar.extractfile('pkglist.json'))
                    for identity in current['identities']:
                        if ':' not in identity['ref']:
                            continue
                        full = identity['ref']
                        name, revisions = full.split('#', 1)
                        rrev, package = revisions.split(':')
                        package_id, prev = package.split('#')
                        exported = listing[name]['revisions'][rrev]['packages'][package_id]['revisions'][prev]['package_folder']
                        members = [m for m in tar.getmembers() if m.name.startswith(exported + '/') and m.isfile()]
                        expected_files = {p.relative_to(paths[full]).as_posix(): p.read_bytes()
                                          for p in Path(paths[full]).rglob('*') if p.is_file()}
                        actual_files = {m.name[len(exported) + 1:]: tar.extractfile(m).read() for m in members}
                        self.assertEqual(actual_files, expected_files)
                        if os.name != 'nt':
                            for member in [tar.getmember(exported), *members]:
                                suffix = member.name[len(exported):].lstrip('/')
                                original_stat = (Path(paths[full]) / suffix).stat()
                                self.assertEqual(member.mode, original_stat.st_mode & 0o7777)
                                self.assertEqual((member.uid, member.gid),
                                                 (original_stat.st_uid, original_stat.st_gid))
                        checked.append({'reference': full, 'files': len(actual_files)})
                current = capture()
                conan(['remove', pref, '-c'])
                self.assertTrue(decision(current)['publish'])
                output = os.environ.get('VEYR_SNAPSHOT_FIXTURE_OUTPUT')
                if output:
                    target = Path(output)
                    target.mkdir(parents=True, exist_ok=True)
                    shutil.copy2(archive, target / 'actual-conan-export.tgz')
                    (target / 'scope-proof.json').write_text(json.dumps({
                        'conan': '2.33.0', 'distinct_finalized_immutable': True,
                        'archive_package_trees': checked, 'commands': commands,
                        'immutable_changes_detected': True, 'corruption_rejected': True,
                        'unchanged_skip': True, 'multiple_revisions': len(prefs)}, indent=2) + '\n')
                print('Real Conan scope proof: distinct finalized/immutable folders; 3 exported package trees; '
                      'unchanged skip, immutable mutation, integrity rejection, revision addition/removal verified')


class WorkflowTests(unittest.TestCase):
    def setUp(self):
        self.text = (SCRIPT.parents[1] / '.github/workflows/build-foundation.yml').read_text()

    def test_mandatory_integrity_setup_and_native_matrix_remain(self):
        self.assertIn('Validate and observe imported Conan export scope', self.text)
        self.assertIn('scripts/ci_conan_snapshot.py observe', self.text)
        self.assertIn('scripts/ci_conan_snapshot.py decide', self.text)
        self.assertLess(self.text.index('scripts/ci_conan_snapshot.py observe'), self.text.index('- name: Explicit Conan dependency setup'))
        self.assertLess(self.text.index('scripts/ci_conan_snapshot.py decide'), self.text.index('cache save'))
        self.assertIn('if ($decision.publish -eq $true)', self.text)
        for name in ('Native cache-only configure', 'Native incremental build', 'Native test',
                     'Native verify', 'Foundation prepared native acceptance',
                     'Foundation acquisition-disabled empty-cache rejection'):
            self.assertIn('- name: ' + name, self.text)
        self.assertNotIn('ci_scope.py', self.text)
        self.assertNotIn('pull_request_target:', self.text)

    def test_exact_key_confirmation_is_early_read_only_and_not_a_restore(self):
        self.assertIn('id: cache_confirm', self.text)
        self.assertIn('lookup-only: true', self.text)
        self.assertIn('SNAPSHOT_EXACT_HIT: ${{ steps.cache_confirm.outputs.cache-hit }}', self.text)
        self.assertLess(self.text.index('id: cache_confirm'), self.text.index('- name: Native cache-only configure'))
        self.assertIn('scripts/ci_conan_snapshot.py report', self.text)

    def test_writer_guard_independent_of_helper_output(self):
        save = self.text.split('id: cache_save\n', 1)[1].split('uses:', 1)[0]
        self.assertIn("steps.export_cache.outputs.publish == 'true'", save)
        self.assertIn("github.ref == 'refs/heads/main'", save)
        self.assertIn("github.event_name == 'push'", save)
        self.assertIn("github.event_name == 'workflow_dispatch'", save)


if __name__ == '__main__':
    unittest.main()
