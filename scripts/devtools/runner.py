"""Single-child command supervision. Quiet checkpoints never imply a timeout."""

import codecs
from dataclasses import dataclass
import os
from pathlib import Path
import queue
import signal
import subprocess
import sys
import threading
import time


COMMAND_LOCK = threading.Lock()


@dataclass(frozen=True)
class Liveness:
    state: str
    detail: str
    snapshot: object = None


def observe_liveness(process, previous):
    if sys.platform != "linux":
        return Liveness("unknown", "Native telemetry unavailable; inspect child CPU/I/O and build artifacts")
    snapshot = {}
    try:
        for directory in Path("/proc").iterdir():
            if not directory.name.isdigit():
                continue
            try:
                fields = (directory / "stat").read_text().rsplit(")", 1)[1].split()
                if int(fields[2]) != process.pid:
                    continue
                counters = dict(line.split(": ", 1) for line in (directory / "io").read_text().splitlines())
                snapshot[directory.name] = (fields[19], int(fields[11]) + int(fields[12]),
                                            sum(int(counters[name]) for name in
                                                ("read_bytes", "write_bytes", "rchar", "wchar")))
            except (OSError, ValueError, IndexError, KeyError):
                continue
    except OSError:
        pass
    if previous is not None and snapshot:
        for pid, counters in snapshot.items():
            old = previous.get(pid)
            if old is None or counters[0] != old[0] or counters[1] > old[1] or counters[2] > old[2]:
                return Liveness("progress", "Child-group CPU/I/O or child identity moved", snapshot)
    return Liveness("unknown", "No proven progress; inspect child state/CPU/I/O and build artifacts. "
                    "PID existence or unchanged counters do not prove a stall", snapshot)


def stop_tree(process):
    """One stop action per failure, with platform-native tree/group handling."""
    if os.name == "nt":
        if process.poll() is None:
            result = subprocess.run(["taskkill", "/PID", str(process.pid), "/T", "/F"],
                                    capture_output=True, text=True, check=False)
            if result.returncode and process.poll() is None:
                raise RuntimeError(f"Cannot stop Windows child tree: {result.stdout}{result.stderr}")
    else:
        try:
            os.killpg(process.pid, signal.SIGTERM)
        except ProcessLookupError:
            pass
        try:
            process.wait(timeout=5)
        except subprocess.TimeoutExpired:
            pass
        # The group can outlive its root. Kill any remaining group members.
        try:
            os.killpg(process.pid, signal.SIGKILL)
        except ProcessLookupError:
            pass
    process.wait(timeout=5)


def run_command(argv, cwd, env, *, capture_output=False, quiet_seconds=60,
                hard_timeout=None, observer=observe_liveness, clock=time.monotonic,
                popen=subprocess.Popen, stopper=stop_tree):
    if quiet_seconds <= 0 or (hard_timeout is not None and hard_timeout <= 0):
        raise ValueError("Supervision intervals must be positive")
    if not COMMAND_LOCK.acquire(blocking=False):
        raise RuntimeError("Conflicting active command; no second child started")
    process, reader = None, None
    started = last_output = last_inspection = 0
    last_visible, captured = "", []
    messages = queue.Queue()
    decoder = codecs.getincrementaldecoder("utf-8")(errors="replace")

    def forward(chunk):
        nonlocal last_visible, last_output
        text = decoder.decode(chunk)
        if text:
            print(text, end="", flush=True)
            last_visible = (last_visible + text)[-4096:]
            if capture_output:
                captured.append(text)
        last_output = clock()

    def read_output():
        try:
            while chunk := process.stdout.read1(4096):
                messages.put(chunk)
        except OSError as error:
            messages.put(error)
        finally:
            messages.put(None)

    try:
        started = last_output = last_inspection = clock()
        print(f"command: {list(argv)!r}; cwd={cwd}", flush=True)
        options = {"creationflags": getattr(subprocess, "CREATE_NEW_PROCESS_GROUP", 0x200)} \
            if os.name == "nt" else {"start_new_session": True}
        process = popen(list(argv), cwd=cwd, env=env, stdout=subprocess.PIPE,
                        stderr=subprocess.STDOUT, **options)
        previous = observer(process, None).snapshot
        reader = threading.Thread(target=read_output, daemon=True)
        reader.start()
        eof = False
        while not (eof and process.poll() is not None):
            try:
                message = messages.get(timeout=0.05)
                if isinstance(message, OSError):
                    raise message
                if message is None:
                    eof = True
                else:
                    forward(message)
            except queue.Empty:
                pass
            now = clock()
            code = process.poll()
            if code is not None and code != 0:
                raise subprocess.CalledProcessError(code, list(argv))
            if code is None and hard_timeout is not None and now - started >= hard_timeout:
                raise subprocess.TimeoutExpired(list(argv), hard_timeout)
            if now - last_output >= quiet_seconds and now - last_inspection >= quiet_seconds:
                observation = observer(process, previous)
                previous, last_inspection = observation.snapshot, now
                print(f"supervisor: pid={process.pid}; elapsed={now-started:.1f}s; "
                      f"output-idle={now-last_output:.1f}s; liveness={observation.state}: "
                      f"{observation.detail}", flush=True)
                if observation.state == "stalled":
                    raise subprocess.CalledProcessError(125, list(argv),
                                                        stderr=f"Confirmed stall: {observation.detail}")
                if observation.state not in {"progress", "unknown"}:
                    raise RuntimeError(f"Invalid liveness observation: {observation.state}")
        tail = decoder.decode(b"", final=True)
        if tail:
            print(tail, end="", flush=True)
            captured.append(tail)
        return subprocess.CompletedProcess(list(argv), process.returncode,
                                           "".join(captured) if capture_output else None)
    except BaseException as error:
        if process is not None:
            try:
                stopper(process)
            except Exception as stop_error:
                print(f"supervisor: child stop failed: {stop_error}; inspect tree manually",
                      file=sys.stderr, flush=True)
            if reader is not None:
                reader.join(timeout=1)
            while not messages.empty():
                message = messages.get_nowait()
                if isinstance(message, bytes):
                    forward(message)
        print(f"supervisor stop: command={list(argv)!r}; cwd={cwd}; "
              f"elapsed={clock()-started:.1f}s; output-idle={clock()-last_output:.1f}s; reason={error}; "
              f"last-output={last_visible!r}. No retry or cleanup.", file=sys.stderr, flush=True)
        raise
    finally:
        if process is not None and (reader is None or not reader.is_alive()):
            process.stdout.close()
        COMMAND_LOCK.release()
