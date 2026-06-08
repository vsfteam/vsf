"""Multi-baud UART TX validation via logic analyzer capture.

Test parameters are read from application/component/vsf-test/test_params.yml
so the script does not depend on firmware output for correctness.

Two-phase: `run()` triggers + waits for firmware completion; `decode()` runs
after the shared LA capture is stopped and validates per-case payloads.
"""

from dataclasses import dataclass
from vsf_bench import read_framework_windows, SerialInstrument, load_test_params
from vsf_bench.capabilities.logic_analyzer import LogicAnalyzer
from vsf_bench.utils import batch_decode_uart, parse_uart_csv, read_csv_rows
from vsf_bench.config import UARTConfig


RP2040_CLK_PERI = 125_000_000
MIN_BAUDRATE = RP2040_CLK_PERI // (16 * 65535)
MAX_BAUDRATE = RP2040_CLK_PERI // 16


def _expect_pass(baud: int) -> bool:
    return baud != 0 and MIN_BAUDRATE <= baud <= MAX_BAUDRATE


def _gen_pattern(size: int) -> bytes:
    """Incrementing-counter pattern: byte[i] = i & 0xFF."""
    return bytes(i & 0xFF for i in range(size))


@dataclass(frozen=True)
class Case:
    idx: int
    baud: int
    data_size_bytes: int


def _parse_cases(scenario: dict) -> list[Case]:
    cases: list[Case] = []
    for case in scenario.get("cases", []):
        cases.append(Case(
            idx=int(case["idx"]),
            baud=int(case["baudrate"]),
            data_size_bytes=int(case.get("data_size_bytes", 0)),
        ))
    return cases


def run(serial: SerialInstrument, test_params_yml=None):
    params = load_test_params(test_params_yml)
    scenario = params.get("tx_baud", {})
    timeout_s = float(scenario.get("timeout_s", 1.5))
    serial.expect_test_summary("usart_baud", timeout=timeout_s)


def decode(adapter: LogicAnalyzer, channels: dict, capture_path: Path,
           decode_start_ns: int | None = None,
           decode_end_ns: int | None = None, test_params_yml=None) -> None:
    params = load_test_params(test_params_yml)
    scenario = params.get("tx_baud", {})
    cases = _parse_cases(scenario)
    assert len(cases) > 0, "No cases found in test_params"

    dut_ch = channels.get(scenario.get("dut", {}).get("channel", "uart1_tx"))
    default_payload = scenario.get("payload", "Hello VSF\r\n").encode()
    out_dir = capture_path.parent

    windows = read_framework_windows(adapter, channels, capture_path, "usart_baud",
        decode_start_ns=decode_start_ns, decode_end_ns=decode_end_ns,
    )
    window_by_idx = {w.case_idx: w for w in windows}

    for c in cases:
        if _expect_pass(c.baud):
            assert c.idx in window_by_idx, f"CASE {c.idx} baud={c.baud}: window missing"

    pass_baudrates = sorted({c.baud for c in cases if _expect_pass(c.baud)})
    batch_decode_uart(adapter, capture_path, [
        (dut_ch, baud, decode_start_ns, decode_end_ns,
         out_dir / f"baud_full_{baud}.csv", "none", 8, 1.0)
        for baud in pass_baudrates
    ])

    for c in cases:
        if not _expect_pass(c.baud):
            print(f"[PASS] CASE {c.idx}  baud={c.baud:>7}  expected fail (init error)")
            continue
        w = window_by_idx[c.idx]
        rows = read_csv_rows(out_dir / f"baud_full_{c.baud}.csv")
        got = bytes(b for t, b in rows if w.start_ns <= t < w.end_ns)
        payload = _gen_pattern(c.data_size_bytes) if c.data_size_bytes > 0 else default_payload
        assert got == payload, (
            f"CASE {c.idx} baud={c.baud}: expected {payload!r}, got {got!r}"
        )
        print(f"[PASS] CASE {c.idx}  baud={c.baud:>7}  {got!r}")
