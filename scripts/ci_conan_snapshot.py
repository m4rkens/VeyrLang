"""Read-only Conan export-scope observations and auditable CI publication decisions.

Conan alone restores/exports packages. No storage paths are guessed. A narrow
2.33.0-only adapter uses cache save's own immutable package layout accessor.
Unsupported observations never authorize an unchanged-snapshot skip.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import stat
import subprocess
import sys


class SnapshotError(ValueError):
    pass


def failure_context(error, reference, role):
    # Do not copy arbitrary stderr, exception text, paths or credentials to receipts.
    if isinstance(error, subprocess.CalledProcessError):
        classification = 'conan-command-failed'
        detail = f'reviewed Conan query exited {error.returncode}; inspect the selected tool and scope'
    elif isinstance(error, OSError):
        classification = 'filesystem-or-tool-unavailable'
        detail = 'scope could not be read; check access and isolated-home/tool availability'
    elif isinstance(error, SnapshotError):
        classification = 'unsupported-scope'
        detail = 'scope or pinned response contract not established; conservative export required'
    else:
        classification = 'unsupported-response'
        detail = 'response could not be interpreted under the pinned schema'
    # References come from validated inventory; escaping also bounds hostile query input.
    return {'reference': json.dumps(reference, ensure_ascii=True)[1:-1][:200],
            'role': role, 'classification': classification, 'detail': detail[:160]}


def pinned_immutable_paths(refs, home):
    """Run only in the reviewed Conan venv; no CLI's finalized-path substitution.

    Pinned CacheAPI.save constructs PkgCache(home, global_conf), then calls
    pkg_layout(full_pref).package(). Use those exact Conan accessors, not a
    reconstructed cache layout. This internal coupling is intentionally version-bound.
    No ConanAPI initialization/migration, graph evaluation or package writes.
    """
    from importlib.metadata import version
    if version('conan') != '2.33.0':
        raise SnapshotError('unsupported Conan adapter version')
    from conan.api.model import PkgReference
    from conan.internal.cache.cache import PkgCache
    from conan.internal.model.conf import load_global_conf
    if not home.is_absolute() or not home.is_dir() or not isinstance(refs, list) or not refs:
        raise SnapshotError('initialized isolated home and full package references required')
    cache = PkgCache(str(home), load_global_conf(str(home)))
    paths = {}
    for ref in refs:
        pref = PkgReference.loads(ref)
        if not pref.ref.revision or not pref.package_id or not pref.revision:
            raise SnapshotError('exact package revisions required')
        paths[ref] = cache.pkg_layout(pref).package()
    return {'schema': 1, 'conan': '2.33.0',
            'resolver': 'PkgCache.pkg_layout(full_pref).package()', 'paths': paths}


def immutable_paths(executable, refs):
    # Same isolated interpreter binding already verified by devtools.doctor.
    launcher = Path(executable)
    python = launcher.with_name('python.exe' if launcher.suffix == '.exe' else 'python')
    raw = json.loads(subprocess.check_output(
        [str(python), '-I', '-B', str(Path(__file__).resolve()), 'immutable-paths'],
        input=json.dumps(refs), stderr=subprocess.PIPE, encoding='utf-8', errors='strict'))
    if (set(raw) != {'schema', 'conan', 'resolver', 'paths'} or raw['schema'] != 1
            or raw['conan'] != '2.33.0' or raw['resolver'] != 'PkgCache.pkg_layout(full_pref).package()'
            or not isinstance(raw['paths'], dict) or set(raw['paths']) != set(refs)):
        raise SnapshotError('unsupported immutable package resolution')
    return raw['paths']


def digest(path):
    result = hashlib.sha256()
    with path.open('rb') as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b''):
            result.update(block)
    return result.hexdigest()


def tree_digest(root, home):
    root = Path(root)
    if (not root.is_absolute() or not root.is_dir() or root.is_symlink()
            or not root.resolve().is_relative_to(home.resolve())):
        raise SnapshotError('export scope outside isolated home or redirected root')
    result, count = hashlib.sha256(), 0

    def visit(path):
        nonlocal count
        before = path.lstat()
        mode = stat.S_IMODE(before.st_mode)
        owner = [getattr(before, 'st_uid', None), getattr(before, 'st_gid', None)]
        name = path.relative_to(root).as_posix()
        if getattr(path, 'is_junction', lambda: False)():
            raise SnapshotError('unsupported junction')
        if stat.S_ISLNK(before.st_mode):
            row = [name, 'link', mode, owner, os.readlink(path)]  # Never dereference cache symlinks.
        elif stat.S_ISDIR(before.st_mode):
            row = [name, 'directory', mode, owner]
        elif stat.S_ISREG(before.st_mode):
            if before.st_nlink != 1:
                raise SnapshotError('unsupported hardlink topology')
            row = [name, 'file', mode, owner, before.st_size, digest(path)]
        else:
            raise SnapshotError('unsupported special file')
        result.update((json.dumps(row, ensure_ascii=True) + '\n').encode())
        count += 1
        if stat.S_ISDIR(before.st_mode):
            for child in sorted(path.iterdir(), key=lambda item: item.name):
                visit(child)
        after = path.lstat()
        if (before.st_mode, before.st_size, before.st_mtime_ns, before.st_ctime_ns, before.st_ino, before.st_dev) != (
                after.st_mode, after.st_size, after.st_mtime_ns, after.st_ctime_ns, after.st_ino, after.st_dev):
            raise SnapshotError('export scope changed while observing')

    visit(root)
    return {'sha256': result.hexdigest(), 'entries': count}


def inventory(raw):
    """Preserve all non-timestamp information; unknown CLI fields disable skipping."""
    if not isinstance(raw, dict) or set(raw) != {'Local Cache'} or not raw['Local Cache']:
        raise SnapshotError('unsupported or empty Conan inventory')
    identities = []
    for name, recipe in sorted(raw['Local Cache'].items()):
        if not re.fullmatch(r'[A-Za-z0-9_.+-]+/[A-Za-z0-9_.+-]+(?:@[A-Za-z0-9_.+-]+/[A-Za-z0-9_.+-]+)?', name):
            raise SnapshotError('unsupported recipe reference')
        if set(recipe) != {'revisions'} or not recipe['revisions']:
            raise SnapshotError('unsupported recipe schema')
        for revision, data in sorted(recipe['revisions'].items()):
            if not re.fullmatch('[0-9a-f]{32}', revision) or set(data) - {'timestamp', 'packages'}:
                raise SnapshotError('unsupported recipe revision schema')
            rref = name + '#' + revision
            identities.append({'ref': rref})
            for package_id, package in sorted(data.get('packages', {}).items()):
                if not re.fullmatch('[0-9a-f]{40}', package_id) or set(package) - {'info', 'revisions'}:
                    raise SnapshotError('unsupported package schema')
                if not package.get('revisions') or not isinstance(package.get('info', {}), dict):
                    raise SnapshotError('incomplete package inventory')
                for prev, metadata in sorted(package['revisions'].items()):
                    if not re.fullmatch('[0-9a-f]{32}', prev) or set(metadata) - {'timestamp'}:
                        raise SnapshotError('unsupported package revision schema')
                    identities.append({'ref': rref + ':' + package_id + '#' + prev,
                                       'info': package.get('info', {})})
    return identities


def scope_path(conan, ref, role):
    args = ['cache', 'path', ref]
    if role not in ('export', 'package'):
        args.append('--folder=' + role)
    args.append('--format=json')
    try:
        raw = json.loads(conan(args))
    except subprocess.CalledProcessError as error:
        label = {'export_source': 'export_sources'}.get(role, role)
        # Only the pinned CLI's exact optional-folder absence is understood.
        display = ref.split('#', 1)[0]
        if ':' in ref:
            display += ':' + ref.split(':', 1)[1].split('#', 1)[0]
        expected = f"ERROR: '{label}' folder does not exist for the reference {display}"
        if role not in ('export', 'package') and error.returncode == 1 and error.stderr.strip() == expected:
            return None
        raise
    if set(raw) != {'cache_path'} or not isinstance(raw['cache_path'], str):
        raise SnapshotError('unsupported Conan path response')
    return Path(raw['cache_path'])


def observe(conan, home, archive, binding, immutable=None):
    if not archive.is_file():
        raise SnapshotError('reported restored archive is missing')
    result = {'schema': 2, 'binding': dict(binding), 'archive_sha256': digest(archive),
              'status': 'unsupported', 'reason': 'observation incomplete'}
    ref, role = '<inventory>', 'version'
    try:
        if conan(['--version']).strip() != 'Conan version 2.33.0':
            raise SnapshotError('unsupported Conan version')
        role = 'inventory'
        identities = inventory(json.loads(conan(['list', '*#*:*#*', '--cache', '--format=json'])))
        scopes = {}
        for identity in identities:
            ref = identity['ref']
            roles = ('package', 'metadata') if ':' in ref else ('export', 'export_source', 'source', 'metadata')
            for role in roles:
                if role == 'package':
                    role = 'immutable-package'
                    if immutable is None:
                        raise SnapshotError('immutable resolver unavailable')
                    paths = immutable([ref])
                    if (not isinstance(paths, dict) or set(paths) != {ref}
                            or not isinstance(paths[ref], str) or not paths[ref]):
                        raise SnapshotError('immutable package path unavailable')
                    path = Path(paths[ref])
                    role = 'package'
                else:
                    path = scope_path(conan, ref, role)
                scopes[ref + '/' + role] = None if path is None else tree_digest(path, home)
        result.update(status='supported', reason='complete-export-scope-observed',
                      identities=identities, scopes=scopes)
    except (ValueError, TypeError, KeyError, AttributeError, OSError,
            RecursionError, subprocess.CalledProcessError) as error:
        result['failure'] = failure_context(error, ref, role)
        result['reason'] = 'unsupported export-scope observation: ' + result['failure']['classification']
    return result


def trusted_writer(event, ref):
    return event in ('push', 'workflow_dispatch') and ref == 'refs/heads/main'


def decide(*, event, ref, cold, setup, integrity, imported, archive, binding, before, capture):
    result = {'schema': 1, 'event': event, 'ref': ref, 'binding': dict(binding), 'publish': False}
    def finish(reason, publish=False):
        return dict(result, reason=reason, publish=publish)
    if cold:
        return finish('cold-build-cache-disabled')
    if not setup or not integrity:
        raise SnapshotError('successful setup and integrity are mandatory')
    matched = binding.get('matched', '')
    if matched and (not imported or not archive.is_file()):
        raise SnapshotError('reported hit missing archive or successful import')
    if matched and (not binding.get('prefix') or not matched.startswith(binding['prefix'])):
        raise SnapshotError('incompatible restored cache key')
    if not trusted_writer(event, ref):
        return finish('not-trusted-main-writer')
    if not matched:
        return finish('no-restored-snapshot', True)
    if before.get('archive_sha256') and digest(archive) != before['archive_sha256']:
        raise SnapshotError('restored archive changed during setup')
    if (before.get('schema') != 2 or before.get('status') != 'supported' or before.get('binding') != binding
            or not isinstance(before.get('archive_sha256'), str)
            or not re.fullmatch('[0-9a-f]{64}', before['archive_sha256'])):
        return finish('restored-inventory-unavailable-or-unsupported', True)
    after = capture()
    if after.get('status') != 'supported' or after.get('schema') != 2 or after.get('binding') != binding:
        return finish('final-inventory-unavailable-or-unsupported', True)
    if after.get('archive_sha256') != before['archive_sha256']:
        raise SnapshotError('restored archive changed during final observation')
    for key in ('identities', 'scopes'):
        if not before.get(key) or not after.get(key):
            return finish('incomplete-export-scope-inventory', True)
    if before['identities'] != after['identities'] or before['scopes'] != after['scopes']:
        return finish('export-scope-changed', True)
    return finish('validated-export-scope-unchanged')


def publication_result(requested, save, lookup, hit):
    if not isinstance(requested, bool):
        raise SnapshotError('invalid publication decision type')
    if not requested:
        return 'not-requested'
    if save != 'success' or lookup != 'success' or hit != 'true':
        raise SnapshotError('snapshot publication not confirmed by exact-key lookup')
    return 'confirmed'


def write_json(path, value):
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(value, indent=2, ensure_ascii=True) + '\n', encoding='utf-8')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('operation', choices=('observe', 'decide', 'report', 'immutable-paths'))
    parser.add_argument('--directory', type=Path, default=Path('.veyr-dev/ci'))
    args = parser.parse_args()
    directory = args.directory
    env = os.environ
    if args.operation == 'immutable-paths':
        try:
            print(json.dumps(pinned_immutable_paths(json.load(sys.stdin), Path(env['CONAN_HOME']))))
            return 0
        except Exception:
            # Parent records the exact affected reference and role; never forward raw internals.
            print('Pinned immutable package query unavailable', file=sys.stderr)
            return 1
    decision_file = directory / 'cache-decision.json'
    try:
        if args.operation == 'report':
            reason = 'cold-build-cache-disabled' if env.get('SNAPSHOT_COLD') == 'true' else 'decision-not-reached'
            decision = json.loads(decision_file.read_text()) if decision_file.is_file() else {'publish': False, 'reason': reason}
            status = publication_result(decision['publish'], env.get('SNAPSHOT_SAVE_RESULT', ''),
                                        env.get('SNAPSHOT_LOOKUP_RESULT', ''), env.get('SNAPSHOT_EXACT_HIT', ''))
            write_json(directory / 'cache-publication.json', dict(decision, publication=status))
            return 0
        archive = Path('.veyr-dev/conan-cache/dependencies.tgz')
        binding = {key: env.get('SNAPSHOT_' + key.upper(), '')
                   for key in ('prefix', 'matched', 'environment', 'profile', 'config')}
        executable, home = env['VEYR_CONAN_EXECUTABLE'], Path(env['CONAN_HOME'])
        if not Path(executable).is_absolute() or not home.is_absolute():
            raise SnapshotError('absolute reviewed Conan executable and isolated home required')
        def conan(argv):
            return subprocess.check_output([executable, *argv], stderr=subprocess.PIPE,
                                           encoding='utf-8', errors='strict')
        def capture():
            return observe(conan, home, archive, binding,
                           immutable=lambda refs: immutable_paths(executable, refs))
        before_file = directory / 'cache-inventory-before.json'
        if args.operation == 'observe':
            write_json(before_file, capture())
        else:
            try:
                before = json.loads(before_file.read_text())
                if not isinstance(before, dict):
                    before = {}
            except (OSError, ValueError):
                before = {}
            def capture_after():
                after = capture()
                write_json(directory / 'cache-inventory-after.json', after)
                return after
            decision = decide(event=env['GITHUB_EVENT_NAME'], ref=env['GITHUB_REF'],
                              cold=env.get('SNAPSHOT_COLD') == 'true',
                              setup=env.get('SNAPSHOT_SETUP_RESULT') == 'success',
                              integrity=env.get('SNAPSHOT_INTEGRITY_RESULT') == 'success',
                              imported=env.get('SNAPSHOT_IMPORT_RESULT') == 'success',
                              archive=archive, binding=binding, before=before, capture=capture_after)
            write_json(decision_file, decision)
            with open(env['GITHUB_OUTPUT'], 'a', encoding='utf-8') as output:
                output.write('publish=' + str(decision['publish']).lower() + '\n')
            print(json.dumps(decision, ensure_ascii=True))
        return 0
    except (SnapshotError, OSError, ValueError, KeyError, TypeError) as error:
        name = 'cache-publication.json' if args.operation == 'report' else 'cache-decision-error.json'
        write_json(directory / name, {'status': 'failed', 'reason': str(error)})
        print('Snapshot operation failed: ' + str(error))
        return 1


if __name__ == '__main__':
    raise SystemExit(main())
