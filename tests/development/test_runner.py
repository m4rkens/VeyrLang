"""Fast supervision fixtures; only short isolated Python children, no native builds."""

import contextlib
import io
import os
from pathlib import Path
import subprocess
import sys
import unittest
from unittest.mock import Mock, patch

sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "scripts"))
from devtools import runner


class FastClock:
    def __init__(self):
        self.now = 0

    def __call__(self):
        self.now += 65
        return self.now


class RunnerTests(unittest.TestCase):
    def setUp(self):
        self.out, self.errors = io.StringIO(), io.StringIO()
        self.spawns, self.stops, self.observations = [], [], []
        self.addCleanup(patch.stopall)
        patch("sys.stdout", self.out).start()
        patch("sys.stderr", self.errors).start()

    def spawn(self, argv, **kwargs):
        self.spawns.append((argv, kwargs))
        return subprocess.Popen(argv, **kwargs)

    def stop(self, process):
        self.stops.append(process.pid)
        runner.stop_tree(process)

    def observation(self, state):
        def observe(process, previous):
            self.observations.append((process.pid, previous))
            return runner.Liveness(state, "fixture CPU/I/O observation", len(self.observations))
        return observe

    def invoke(self, source, **kwargs):
        return runner.run_command([sys.executable, "-I", "-u", "-c", source],
                                  cwd=Path.cwd(), env=dict(os.environ),
                                  popen=self.spawn, stopper=self.stop, **kwargs)

    def test_real_short_quiet_child_streams_resume_and_spawns_once(self):
        result = self.invoke("import time; print('begin'); time.sleep(.15); print('resumed')",
                             quiet_seconds=.03, capture_output=True)
        self.assertEqual(result.returncode, 0)
        self.assertIn("begin\nresumed\n", result.stdout)
        self.assertIn("resumed", self.out.getvalue())
        self.assertIn("supervisor: pid=", self.out.getvalue())
        self.assertEqual(len(self.spawns), 1)
        self.assertEqual(self.stops, [])
        self.assertTrue(self.spawns[0][1]["start_new_session"] if os.name != "nt"
                        else self.spawns[0][1]["creationflags"])
        self.assertNotIn("shell", self.spawns[0][1])

    def test_simulated_60_second_progress_keeps_waiting(self):
        result = self.invoke("import time; time.sleep(.15); print('done')", clock=FastClock(),
                             observer=self.observation("progress"))
        self.assertEqual(result.returncode, 0)
        self.assertGreater(len(self.observations), 1)
        self.assertIn("liveness=progress", self.out.getvalue())
        self.assertEqual(len(self.spawns), 1)
        self.assertEqual(self.stops, [])

    def test_unknown_telemetry_continues_without_timeout(self):
        result = self.invoke("import time; time.sleep(.15)", clock=FastClock(),
                             observer=self.observation("unknown"))
        self.assertEqual(result.returncode, 0)
        self.assertIn("liveness=unknown", self.out.getvalue())
        self.assertEqual(len(self.spawns), 1)
        self.assertEqual(self.stops, [])

    def test_confirmed_stall_stops_reports_once_and_never_retries(self):
        with self.assertRaises(subprocess.CalledProcessError) as caught:
            self.invoke("import time; time.sleep(5)", clock=FastClock(),
                        observer=self.observation("stalled"))
        self.assertEqual(caught.exception.returncode, 125)
        self.assertEqual(len(self.spawns), 1)
        self.assertEqual(len(self.stops), 1)
        self.assertEqual(self.errors.getvalue().count("supervisor stop:"), 1)
        self.assertIn("No retry or cleanup", self.errors.getvalue())
        self.assertIn("output-idle=", self.errors.getvalue())

    def test_nonzero_exit_preserves_output_and_stops_once(self):
        with self.assertRaises(subprocess.CalledProcessError) as caught:
            self.invoke("import sys; print('actual failure', file=sys.stderr); sys.exit(7)")
        self.assertEqual(caught.exception.returncode, 7)
        self.assertIn("actual failure", self.errors.getvalue())
        self.assertEqual(len(self.stops), 1)
        self.assertEqual(len(self.spawns), 1)

    def check_mocked_exit(self, codes, expected):
        """Control EOF/exit timing without launching a child or racing threads."""
        self.out.seek(0)
        self.out.truncate()
        self.errors.seek(0)
        self.errors.truncate()
        process = Mock(pid=123, returncode=None)
        process.stdout.read1.side_effect = [b"stdout drained\nstderr dr", b"ained\n", b""]
        exits = iter(codes)

        def poll():
            process.returncode = next(exits)
            return process.returncode

        process.poll.side_effect = poll
        popen, stopper = Mock(return_value=process), Mock()
        thread = Mock()
        thread.is_alive.return_value = False

        def make_reader(*, target, daemon):
            self.assertTrue(daemon)
            thread.start.side_effect = target
            return thread

        argv = ["mock-child", "exit-fixture"]
        with patch.object(runner.threading, "Thread", side_effect=make_reader):
            if expected:
                with self.assertRaises(subprocess.CalledProcessError) as caught:
                    runner.run_command(argv, cwd=Path.cwd(), env={}, popen=popen,
                                       stopper=stopper, capture_output=True,
                                       observer=self.observation("unknown"))
                self.assertEqual(caught.exception.returncode, expected)
                self.assertEqual(caught.exception.cmd, argv)
                stopper.assert_called_once_with(process)
                self.assertEqual(self.errors.getvalue().count("supervisor stop:"), 1)
                self.assertIn("stdout drained\\nstderr drained\\n", self.errors.getvalue())
            else:
                result = runner.run_command(argv, cwd=Path.cwd(), env={}, popen=popen,
                                            stopper=stopper, capture_output=True,
                                            observer=self.observation("unknown"))
                self.assertEqual(result.returncode, 0)
                self.assertEqual(result.stdout, "stdout drained\nstderr drained\n")
                stopper.assert_not_called()
                self.assertEqual(self.errors.getvalue(), "")
        self.assertIn("stdout drained\nstderr drained\n", self.out.getvalue())
        popen.assert_called_once()
        self.assertEqual(popen.call_args.args[0], argv)
        self.assertIs(popen.call_args.kwargs["stderr"], subprocess.STDOUT)
        thread.start.assert_called_once()
        process.stdout.close.assert_called_once()
        self.assertFalse(runner.COMMAND_LOCK.locked())

    def test_mocked_nonzero_exit_before_eof_preserves_drained_output(self):
        self.check_mocked_exit([7], 7)

    def test_mocked_nonzero_exit_at_eof_preserves_drained_output(self):
        self.check_mocked_exit([None, None, 7], 7)

    def test_mocked_nonzero_exit_during_eof_condition_is_reported(self):
        # Two output chunks, then EOF with an active child; the next poll exits.
        self.check_mocked_exit([None, None, None, 7], 7)

    def test_mocked_eof_before_delayed_final_exit_is_reported(self):
        self.check_mocked_exit([None, None, None, None, None, 7], 7)

    def test_mocked_successful_exit_before_or_after_eof_remains_zero(self):
        for codes in ([0, 0, 0, 0], [None, None, None, 0],
                      [None, None, None, None, 0, 0]):
            with self.subTest(codes=codes):
                self.check_mocked_exit(codes, 0)

    def test_owner_interrupt_stops_tree_once_and_releases_guard(self):
        def interrupt(process, previous):
            raise KeyboardInterrupt()
        with self.assertRaises(KeyboardInterrupt):
            self.invoke("import time; time.sleep(5)", observer=interrupt)
        self.assertEqual(len(self.stops), 1)
        self.assertFalse(runner.COMMAND_LOCK.locked())

    def test_explicit_hard_timeout_is_distinct_from_quiet_checkpoint(self):
        with self.assertRaises(subprocess.TimeoutExpired):
            self.invoke("import time; time.sleep(5)", hard_timeout=.03, quiet_seconds=60)
        self.assertEqual(len(self.stops), 1)
        self.assertIn("timed out", self.errors.getvalue())

    def test_process_conflict_starts_no_second_child(self):
        runner.COMMAND_LOCK.acquire()
        try:
            with self.assertRaisesRegex(RuntimeError, "Conflicting active command"):
                self.invoke("print('must not run')")
        finally:
            runner.COMMAND_LOCK.release()
        self.assertEqual(self.spawns, [])

    def test_interrupt_before_spawn_releases_guard(self):
        with self.assertRaises(KeyboardInterrupt):
            self.invoke("print('must not run')", clock=Mock(side_effect=KeyboardInterrupt()))
        self.assertEqual(self.spawns, [])
        self.assertFalse(runner.COMMAND_LOCK.locked())

    def test_launch_failure_releases_guard_without_retry(self):
        spawn = Mock(side_effect=FileNotFoundError("missing fixture executable"))
        with self.assertRaises(FileNotFoundError):
            runner.run_command(["missing"], cwd=Path.cwd(), env={}, popen=spawn)
        spawn.assert_called_once()
        self.assertFalse(runner.COMMAND_LOCK.locked())
        self.assertIn("missing fixture executable", self.errors.getvalue())

    def test_stop_error_keeps_original_failure_and_reports_manual_inspection(self):
        with self.assertRaises(subprocess.CalledProcessError) as caught:
            runner.run_command(
                [sys.executable, "-I", "-c", "import sys; sys.exit(7)"], cwd=Path.cwd(),
                env=dict(os.environ), stopper=Mock(side_effect=RuntimeError("fixture stop error")))
        self.assertEqual(caught.exception.returncode, 7)
        self.assertIn("inspect tree manually", self.errors.getvalue())
        self.assertIn("supervisor stop:", self.errors.getvalue())

    def test_windows_spawn_uses_explicit_group_and_tree_stop(self):
        process = Mock(pid=123, returncode=0, stdout=io.BytesIO(b"fixture output\n"))
        process.poll.return_value = 0
        popen = Mock(return_value=process)
        with patch.object(runner.os, "name", "nt"):
            result = runner.run_command(["C:\\Tool with spaces\\conan.exe", "--version"],
                                        cwd="C:\\Checkout with spaces", env={}, popen=popen,
                                        observer=self.observation("unknown"))
            self.assertEqual(result.returncode, 0)
            self.assertEqual(popen.call_args.kwargs["creationflags"], 0x200)
            self.assertNotIn("start_new_session", popen.call_args.kwargs)
            process.poll.return_value = None
            with patch.object(runner.subprocess, "run", return_value=subprocess.CompletedProcess([], 0)) as stop:
                runner.stop_tree(process)
            stop.assert_called_once_with(["taskkill", "/PID", "123", "/T", "/F"],
                                         capture_output=True, text=True, check=False)

    def test_posix_stop_targets_group_even_when_root_has_exited(self):
        process = Mock(pid=123)
        if os.name == "nt":
            self.skipTest("POSIX-only group fixture")
        with patch.object(runner.os, "killpg") as kill:
            runner.stop_tree(process)
        self.assertEqual(kill.call_count, 2)
        self.assertEqual([call.args[0] for call in kill.call_args_list], [123, 123])

    def test_unchanged_or_unavailable_telemetry_never_proves_stall(self):
        process = Mock(pid=-999)
        with patch.object(runner.sys, "platform", "linux"):
            observation = runner.observe_liveness(process, {})
        self.assertEqual(observation.state, "unknown")
        self.assertIn("do not prove a stall", observation.detail)
        with patch.object(runner.sys, "platform", "win32"):
            observation = runner.observe_liveness(process, {})
        self.assertEqual(observation.state, "unknown")
        self.assertIn("inspect", observation.detail)


if __name__ == "__main__":
    unittest.main()
