"""TeeLogger: thread-safe stream logging to terminal + file.

Extracted from devtool.py so vsf-bench can log script events and device
output with ISO timestamps to both the terminal and a log file.

Singleton pattern: ``get_logger()`` / ``init_logger(log_dir)``.
"""

import threading
from datetime import datetime
from pathlib import Path

_logger_instance: "TeeLogger | None" = None


def _format_timestamp():
    return datetime.now().strftime("%Y-%m-%d %H:%M:%S.%f")[:-3]


class TeeLogger:
    """Thread-safe logger for terminal + file output."""

    def __init__(self, log_path: Path | str | None = None):
        self._file = None
        self._lock = threading.Lock()
        if log_path is not None:
            p = Path(log_path)
            p.parent.mkdir(parents=True, exist_ok=True)
            self._file = open(p, "w", encoding="utf-8")

    def _log(self, prefix, message):
        line = f"{_format_timestamp()} {prefix} {message}"
        with self._lock:
            print(line)
            if self._file:
                self._file.write(line + "\n")
                self._file.flush()

    def event(self, message):
        self._log("[vsf-bench]", message)

    def device(self, line_text):
        self._log("[DEV]", line_text)

    def raw(self, text):
        with self._lock:
            print(text)

    def close(self):
        with self._lock:
            if self._file:
                self._file.close()
                self._file = None


def init_logger(log_dir: Path | str | None = None) -> TeeLogger:
    """Create and register the singleton TeeLogger.  Call once at startup."""
    global _logger_instance
    log_path = None
    if log_dir is not None:
        log_path = Path(log_dir) / "console.log"
    _logger_instance = TeeLogger(log_path)
    return _logger_instance


def get_logger() -> TeeLogger:
    """Return the singleton logger.  Raises if not initialized."""
    if _logger_instance is None:
        raise RuntimeError("TeeLogger not initialized — call init_logger() first")
    return _logger_instance
