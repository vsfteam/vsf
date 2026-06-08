"""SerialInstrument — programmatic serial send/expect with audit log."""

import json
import re
import time
from datetime import datetime, timezone
from pathlib import Path

import serial


class SuiteTimeoutError(TimeoutError):
    """Raised when a test suite exceeds its `suite_timeout_s` deadline.

    Distinct from `TimeoutError` so logs clearly distinguish "entire scene
    timed out" from "a single `expect()` timed out".
    """
    pass


class SerialInstrument:
    def __init__(
        self,
        port: str,
        baud: int = 115200,
        audit_log: Path | None = None,
        echo: bool = True,
    ):
        self._port = port
        self._baud = baud
        self._audit_log = audit_log
        self._echo = echo
        self._ser: serial.Serial | None = None
        self._leftover = ""

    def open(self) -> None:
        self._ser = serial.Serial(self._port, self._baud, timeout=0.1)
        time.sleep(0.1)
        self._ser.reset_input_buffer()
        self._leftover = ""

    def close(self) -> None:
        if self._ser and self._ser.is_open:
            self._ser.close()

    def reconnect(self, timeout: float = 10.0) -> None:
        """Close and reopen the serial port, waiting until it becomes available.

        Used after a device reset (e.g. WDT reboot) where the USB CDC/serial
        device disappears and re-enumerates.
        """
        self.close()
        deadline = time.monotonic() + timeout
        while time.monotonic() < deadline:
            try:
                self.open()
                return
            except (serial.SerialException, OSError):
                time.sleep(0.2)
        raise TimeoutError(f"Serial port {self._port} did not reappear within {timeout}s")

    def _echo_print(self, chunk: str) -> None:
        """Print chunk with host timestamp prefix on each line."""
        if not chunk:
            return
        ts = datetime.now().strftime('%Y-%m-%d %H:%M:%S.%f')[:-3]
        # Preserve trailing newline semantics: split into lines, prefix each
        lines = chunk.split('\n')
        for i, line in enumerate(lines):
            if i > 0:
                print('\n', end="")
            # Only prefix non-empty lines and lines that aren't just the final empty split
            if line or i < len(lines) - 1:
                print(f"[{ts}] {line}", end="", flush=True)
            elif i == len(lines) - 1 and not line:
                # chunk ended with '\n' — already printed the newline above
                pass

    def send(self, data: str) -> None:
        assert self._ser is not None
        self._ser.write(data.encode())
        self._ser.flush()
        self._log("send", data)

    def expect(self, pattern: str, timeout: float = 5.0) -> str:
        """Read until pattern matches or timeout. Returns matched line.

        Unconsumed data after the matched line is preserved as leftover and
        prepended to the next read, so no data is silently lost.
        """
        assert self._ser is not None
        deadline = time.monotonic() + timeout
        buf = self._leftover
        self._leftover = ""

        # Check leftover first
        for line in buf.splitlines(keepends=True):
            if re.search(pattern, line):
                remaining = buf[buf.index(line) + len(line):]
                self._leftover = remaining
                self._log("recv", line.rstrip())
                return line.rstrip()

        while time.monotonic() < deadline:
            available = self._ser.in_waiting
            if available > 0:
                chunk = self._ser.read(available).decode(errors="replace")
                buf += chunk
                if self._echo:
                    self._echo_print(chunk)

            for line in buf.splitlines(keepends=True):
                if re.search(pattern, line):
                    remaining = buf[buf.index(line) + len(line):]
                    self._leftover = remaining
                    self._log("recv", line.rstrip())
                    return line.rstrip()

            time.sleep(0.05)

        self._leftover = buf
        self._log("recv", buf.rstrip(), verdict="fail")
        raise TimeoutError(f"Timeout waiting for pattern '{pattern}' in: {buf!r}")

    def read_all(self, timeout: float = 2.0) -> str:
        """Read all available data until timeout expires with no new data."""
        assert self._ser is not None
        deadline = time.monotonic() + timeout
        buf = self._leftover
        self._leftover = ""

        while time.monotonic() < deadline:
            available = self._ser.in_waiting
            if available > 0:
                chunk = self._ser.read(available).decode(errors="replace")
                buf += chunk
                if self._echo:
                    self._echo_print(chunk)
                deadline = time.monotonic() + timeout
            else:
                time.sleep(0.05)

        if buf:
            self._log("recv", buf.rstrip())
        return buf

    def _log(self, direction: str, data: str, verdict: str = "pending") -> None:
        if self._audit_log is None:
            return
        record = {
            "ts": datetime.now(timezone.utc).isoformat(),
            "direction": direction,
            "data": data,
            "verdict": verdict,
        }
        with open(self._audit_log, "a") as f:
            f.write(json.dumps(record, ensure_ascii=False) + "\n")

    def expect_test_summary(self, name: str, timeout: float = 1.5) -> tuple[int, int, int]:
        """Wait for firmware test completion and parse Pass/Fail/Skip summary.

        Returns (passed, failed, skipped). Asserts failed==0 and passed>0.
        """
        self.expect("all test cases completed", timeout=timeout)
        summary = self.expect(r"pass: (\d+), fail: (\d+), skip: (\d+)", timeout=5.0)
        m = re.search(r"pass: (\d+), fail: (\d+), skip: (\d+)", summary)
        assert m is not None, f"Could not parse test summary: {summary!r}"
        passed, failed, skipped = int(m.group(1)), int(m.group(2)), int(m.group(3))
        print(f"[{name}] pass={passed} fail={failed} skip={skipped}")
        assert failed == 0, f"{failed} assertion(s) failed in firmware"
        assert passed > 0, "no cases ran"
        return passed, failed, skipped

    def __enter__(self):
        self.open()
        return self

    def __exit__(self, *_):
        self.close()
