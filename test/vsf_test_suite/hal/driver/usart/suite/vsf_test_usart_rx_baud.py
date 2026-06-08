"""USART RX baud-rate validation: PC sends payload at varying baud rates.

Two-phase: `run()` per-case handshakes (READY → host writes payload, firmware
reads it, emits DONE, prints next READY); `decode()` walks the shared LA
capture, slicing each case's payload on uart1_rx by the firmware-emitted
READY → DONE pair.
"""

from dataclasses import dataclass


RP2040_CLK_PERI = 125_000_000
MIN_BAUDRATE = RP2040_CLK_PERI // (16 * 65535)
MAX_BAUDRATE = RP2040_CLK_PERI // 16


from vsf_bench import read_framework_windows, SerialInstrument, load_test_params
from vsf_bench.capabilities.logic_analyzer import LogicAnalyzer
from vsf_bench.utils import batch_decode_uart, parse_uart_csv, read_csv_rows
from vsf_bench.config import UARTConfig
def _expect_pass(baud: int) -> bool:
    return baud != 0 and MIN_BAUDRATE <= baud <= MAX_BAUDRATE


@dataclass(frozen=True)
class Case:
    idx: int
    baud: int


def _parse_cases(scenario: dict) -> list[Case]:
    cases: list[Case] = []
    for case in scenario.get("cases", []):
        cases.append(Case(
            idx=int(case["idx"]),
            baud=int(case["baudrate"]),
        ))
    return cases


def run(serial: SerialInstrument, test_params_yml=None):
    params = load_test_params(test_params_yml)
    scenario = params.get("rx_baud", {})
    cases = _parse_cases(scenario)
    assert len(cases) > 0, "No cases found in test_params"

    timeout_s = float(scenario.get("timeout_s", 1.5))
    marker_baud = int((params.get("marker", {}) or {}).get("baudrate", 115200))
    dut_port = scenario.get("dut", {}).get("port", "/dev/ttyUSB0")
    payload = scenario.get("payload", "Hello VSF\r\n").encode()

    import serial as pyserial

    aux = pyserial.Serial(dut_port, baudrate=marker_baud, timeout=1)

    for c in cases:
        if not _expect_pass(c.baud):
            continue
        serial.expect(f"usart_rx_baud:CASE:{c.idx}:READY", timeout=timeout_s)
        aux.baudrate = c.baud
        aux.write(payload)
        aux.flush()

    serial.expect_test_summary("usart_rx_baud", timeout=timeout_s)
    aux.close()


def decode(adapter: LogicAnalyzer, channels: dict, capture_path: Path,
           decode_start_ns: int | None = None,
           decode_end_ns: int | None = None, test_params_yml=None) -> None:
    params = load_test_params(test_params_yml)
    scenario = params.get("rx_baud", {})
    cases = _parse_cases(scenario)
    pass_cases = [c for c in cases if _expect_pass(c.baud)]
    if not pass_cases:
        return

    dut_ch = channels.get(scenario.get("dut", {}).get("channel", "uart1_rx"))
    payload = scenario.get("payload", "Hello VSF\r\n").encode()
    out_dir = capture_path.parent

    windows = read_framework_windows(adapter, channels, capture_path, "usart_rx_baud",
        decode_start_ns=decode_start_ns, decode_end_ns=decode_end_ns,
    )
    window_by_idx = {w.case_idx: w for w in windows}
    for c in pass_cases:
        assert c.idx in window_by_idx, f"CASE {c.idx} baud={c.baud}: window missing"

    pass_baudrates = sorted({c.baud for c in pass_cases})
    config_to_csv = {b: out_dir / f"rx_baud_full_{b}.csv" for b in pass_baudrates}
    batch_decode_uart(adapter, capture_path, [
        (dut_ch, b, decode_start_ns, decode_end_ns,
         config_to_csv[b], "none", 8, 1.0)
        for b in pass_baudrates
    ])

    for c in pass_cases:
        w = window_by_idx[c.idx]
        rows = read_csv_rows(config_to_csv[c.baud])
        got = bytes(b for t, b in rows if w.start_ns <= t < w.end_ns)
        assert got == payload, (
            f"CASE {c.idx} baud={c.baud}: expected {payload!r}, got {got!r}"
        )
        print(f"[PASS] CASE {c.idx}  baud={c.baud:>7}  {got!r}")
