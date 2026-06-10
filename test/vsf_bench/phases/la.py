"""LA (Logic Analyzer) capture and decode phases."""

from __future__ import annotations

import shutil
import time
from pathlib import Path

from vsf_bench.board import power_cycle
from vsf_bench.utils.serial import SerialInstrument
from vsf_bench.utils.tee_logger import get_logger as _get_logger


def resolve_la_cli(la_cfg) -> Path:
    cli = Path(la_cfg.cli) if la_cfg.cli else Path("dsview-cli")
    if not la_cfg.cli:
        resolved = shutil.which("dsview-cli")
        if resolved:
            cli = Path(resolved)
    return cli


def la_capture_phase(
    board,
    run_dir: Path,
    duration: float = 30.0,
    channel: str = "CH8",
) -> Path:
    """Power-cycle board, capture via LA, capture serial output.

    All artifacts go under *run_dir*.  Returns the .dsl path.
    """
    la_cfg = board.logic_analyzer
    if la_cfg is None:
        raise RuntimeError(f"Board '{board.name}' has no logic_analyzer config")

    cli = resolve_la_cli(la_cfg)
    capture_path = run_dir / f"la-{channel}.dsl"

    if board.power:
        power_cycle(board)

    ser_log = run_dir / "la-serial.log"
    ser = None
    if board.debug_uart:
        try:
            ser = SerialInstrument(board.debug_uart, board.debug_baudrate)
            ser.open()
        except Exception as e:
            _get_logger().event(f"Serial not available for capture: {e}")

    from vsf_bench.adapters.dsview import DSViewAdapter
    adapter = DSViewAdapter(cli, la_cfg.device, la_cfg.samplerate, {"capture": channel})
    adapter.start(capture_path, duration)
    adapter.wait_until_started()
    _get_logger().event(f"LA capturing {channel} for {duration}s...")

    t0 = time.monotonic()
    serial_buf = ""
    while time.monotonic() - t0 < duration:
        if ser:
            try:
                chunk = ser.read_all(timeout=0.5)
                if chunk:
                    serial_buf += chunk
                    _get_logger().device(chunk.rstrip())
            except Exception:
                pass

    if ser:
        try:
            remaining = ser.read_all(timeout=1.0)
            if remaining:
                serial_buf += remaining
        except Exception:
            pass

    adapter.wait(timeout=30)
    _get_logger().event(f"LA capture done → {capture_path}")

    if ser:
        ser.close()
        if serial_buf:
            normalized = serial_buf.replace("\r\n", "\n").replace("\r", "\n")
            if not normalized.endswith("\n"):
                normalized += "\n"
            ser_log.write_bytes(normalized.encode("utf-8"))
            _get_logger().event(f"Serial log → {ser_log} ({len(normalized)} bytes)")

    return capture_path


def la_decode_phase(
    capture_path: Path,
    channel: str = "CH8",
    baudrate: int = 2_000_000,
    parity_type: str = "none",
    num_data_bits: int = 8,
    num_stop_bits: float = 1.0,
) -> Path:
    """Decode UART from a .dsl capture file.  Returns .txt path."""
    from vsf_bench.adapters.dsview import DSViewAdapter
    from vsf_bench.config.models import UARTConfig
    from vsf_bench.utils.core import parse_uart_csv

    cli = Path("dsview-cli")
    resolved = shutil.which("dsview-cli")
    if resolved:
        cli = Path(resolved)

    adapter = DSViewAdapter(cli, "DSLogic", "10M", {})
    cfg = UARTConfig(baudrate=baudrate, parity_type=parity_type,
                     num_data_bits=num_data_bits, num_stop_bits=num_stop_bits)
    csv = adapter.decode_uart(capture_path, channel, cfg)
    _get_logger().event(f"LA decode CSV → {csv}")
    data = parse_uart_csv(csv)
    text = data.decode("utf-8", errors="replace")
    output = capture_path.parent / f"la-decode-{channel}-{baudrate}.txt"
    output.write_text(text, encoding="utf-8")
    _get_logger().event(f"LA decoded {len(data)} bytes → {output}")
    return output
