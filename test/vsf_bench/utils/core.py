"""Small utilities shared by test scripts and the harness."""

import csv
import time
from pathlib import Path
from typing import Callable

from vsf_bench.utils.serial import SuiteTimeoutError


def run_with_suite_timeout(
    fn: Callable, timeout_s: float, *args, **kwargs
):
    """Call `fn(*args, **kwargs)` and enforce a suite-level deadline.

    If `timeout_s` > 0 and `fn` has not returned within `timeout_s` seconds,
    raises `SuiteTimeoutError`.

    Returns whatever `fn` returns.
    """
    if timeout_s <= 0:
        return fn(*args, **kwargs)

    deadline = time.monotonic() + timeout_s
    result = fn(*args, **kwargs)
    if time.monotonic() > deadline:
        raise SuiteTimeoutError(
            f"Suite exceeded {timeout_s}s deadline (suite_timeout_s)"
        )
    return result


# ---------------------------------------------------------------------------
# LA decode: pure CSV utilities (no hardware dependency)
# ---------------------------------------------------------------------------

def parse_uart_csv(csv_path: Path) -> bytes:
    """Parse a decoded UART CSV and return the byte sequence."""
    rows = read_csv_rows(csv_path)
    return bytes(b for _, b in rows)


def read_csv_rows(csv_path: Path) -> list[tuple[int, int]]:
    """Read decoded UART CSV; return list of ``(time_ns, byte_value)``."""
    if not csv_path.exists():
        raise FileNotFoundError(f"Decoded CSV not found: {csv_path}")

    result = []
    with open(csv_path, newline="", encoding="utf-8") as f:
        reader = csv.reader(f)
        next(reader)  # skip header
        for row in reader:
            if len(row) < 3:
                continue
            time_ns = int(float(row[1]))
            text = row[2].strip()
            if not text:
                continue
            byte_val = _parse_csv_cell(text)
            if byte_val is not None:
                result.append((time_ns, byte_val))
    return result


def _parse_csv_cell(text: str) -> int | None:
    """Parse one CSV cell to a byte value.

    Recognised formats (in priority order):
    1. Single printable ASCII char → ord(char)
    2. ``[HH]`` bracketed hex → int(HH, 16)
    3. Bare 2-char hex ``HH`` → int(HH, 16)
    4. Anything else (annotations like 'Start bit' / 'Stop bit') → None
    """
    if len(text) == 1 and 33 <= ord(text) <= 126:
        return ord(text)
    if len(text) == 4 and text[0] == "[" and text[3] == "]":
        try:
            return int(text[1:3], 16)
        except ValueError:
            return None
    if len(text) == 2 and all(c in "0123456789ABCDEFabcdef" for c in text):
        try:
            return int(text, 16)
        except ValueError:
            return None
    return None


# ---------------------------------------------------------------------------
# batch_decode_uart — convenience for multi-channel UART decode
# ---------------------------------------------------------------------------

def batch_decode_uart(adapter, capture_path: Path, specs: list) -> None:
    """Decode multiple UART configs in one pass.

    Each spec is ``(channel, baudrate, start_ns, end_ns, output_csv,
    parity_type, num_data_bits, num_stop_bits)``.
    """
    from vsf_bench.config.models import UARTConfig

    for ch, baud, s, e, out_csv, parity, data_bits, stop_bits in specs:
        cfg = UARTConfig(baudrate=baud, parity_type=parity,
                         num_data_bits=data_bits, num_stop_bits=stop_bits)
        adapter.decode_uart(capture_path, ch, cfg, s, e)
