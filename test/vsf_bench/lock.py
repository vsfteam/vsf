"""BoardLock — PID-file based mutual exclusion for shared board hardware.

Each board gets one lock file under ``/tmp/vsf-bench-locks/``. The lock
holder writes its PID into the file; contenders check whether that PID is
still alive. Stale locks (dead PID) are transparently cleaned up.

Usage::

    lock = BoardLock("ttyUSB1")
    lock.acquire()          # blocks until available
    # ... test ...
    lock.release()

    # Or as a context manager with optional timeout:
    with BoardLock("ttyUSB1", wait=True, timeout=120):
        ...
"""

import os
import time
from pathlib import Path

import tempfile
LOCK_DIR = Path(tempfile.gettempdir()) / "vsf-bench-locks"


class LockBusyError(RuntimeError):
    """Raised when a lock cannot be acquired in non-blocking mode or
    the wait timeout expires."""


class BoardLock:
    def __init__(self, board_name: str):
        LOCK_DIR.mkdir(parents=True, exist_ok=True)
        self._path = LOCK_DIR / f"{board_name}.lock"
        self._held = False

    # ------------------------------------------------------------------
    def acquire(self, wait: bool = True, timeout: float | None = None) -> bool:
        """Acquire the lock for *board_name*.

        *wait=False*   → raise LockBusyError immediately if locked.
        *wait=True*    → block until available (or *timeout* expires).
        Returns True when the lock was acquired.
        """
        deadline = None
        if timeout is not None:
            deadline = time.monotonic() + timeout

        while True:
            try:
                self._try_acquire()
                self._held = True
                return True
            except LockBusyError:
                if not wait:
                    raise
                if deadline is not None and time.monotonic() >= deadline:
                    raise LockBusyError(
                        f"Timed out waiting for lock on '{self._path.stem}' "
                        f"after {timeout}s"
                    )
                time.sleep(0.5)

    def release(self) -> None:
        """Release the lock. Idempotent — safe to call multiple times."""
        if not self._held:
            return
        try:
            if self._path.exists():
                pid = self._read_pid()
                if pid == os.getpid():
                    self._path.unlink()
        except (OSError, ValueError):
            pass
        finally:
            self._held = False

    @property
    def is_held(self) -> bool:
        return self._held

    # ------------------------------------------------------------------
    # context manager
    def __enter__(self):
        self.acquire(wait=True)
        return self

    def __exit__(self, *_):
        self.release()
        return False

    # ------------------------------------------------------------------
    def _try_acquire(self) -> None:
        """Atomically create the lock file.  Raises LockBusyError if the
        lock is held by a live process, or cleans up a stale lock and
        retries once."""
        try:
            fd = os.open(self._path, os.O_CREAT | os.O_EXCL | os.O_WRONLY, 0o644)
        except FileExistsError:
            self._handle_existing()
            # After cleaning a stale lock, try once more
            try:
                fd = os.open(self._path, os.O_CREAT | os.O_EXCL | os.O_WRONLY, 0o644)
            except FileExistsError:
                raise LockBusyError(
                    f"Board '{self._path.stem}' is locked by another process (pid {self._read_pid()})"
                )
        with os.fdopen(fd, "w") as f:
            f.write(str(os.getpid()))

    def _handle_existing(self) -> None:
        """If the existing lock file's PID is dead, remove it (stale lock)."""
        try:
            pid = self._read_pid()
        except (ValueError, OSError):
            # Unreadable lock file → treat as stale
            self._path.unlink()
            return

        if not _pid_alive(pid):
            # Stale lock — process died without releasing
            self._path.unlink()

    def _read_pid(self) -> int:
        return int(self._path.read_text().strip())


def _pid_alive(pid: int) -> bool:
    """Return True if a process with *pid* exists."""
    try:
        os.kill(pid, 0)
        return True
    except (OSError, ProcessLookupError):
        return False
