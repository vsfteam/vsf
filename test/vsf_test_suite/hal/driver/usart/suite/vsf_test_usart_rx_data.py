"""USART RX data validation: PC sends payload at 115200, firmware verifies via ASSERT.

Two-phase: `run()` per-case handshake (READY → host writes payload, firmware
reads it, emits DONE); `decode()` confirms the on-wire data matches `payload`
within each case's firmware-emitted READY → DONE window.
"""

from dataclasses import dataclass

from vsf_bench.vsf_test.marker import read_framework_windows, SerialInstrument, load_test_params
from vsf_bench.capabilities.logic_analyzer import LogicAnalyzer
from vsf_bench.utils.core import batch_decode_uart, parse_uart_csv, read_csv_rows
from vsf_bench.config.models import UARTConfig


@dataclass(frozen=True)
class Case:
    idx: int
    data_size_bytes: int
    expect_pass: bool


def _gen_pattern(size: int) -> bytes:
    """Incrementing-counter pattern: byte[i] = i & 0xFF."""
    return bytes(i & 0xFF for i in range(size))


def _parse_cases(scenario: dict) -> list[Case]:
    cases: list[Case] = []
    for case in scenario.get("cases", []):
        cases.append(Case(
            idx=int(case["idx"]),
            data_size_bytes=int(case.get("data_size_bytes", 0)),
            expect_pass=bool(case.get("expect_pass", True)),
        ))
    return cases


def run(serial: SerialInstrument, test_params_yml=None):
    params = load_test_params(test_params_yml)
    scenario = params.get("rx_data", {})
    cases = _parse_cases(scenario)
    assert len(cases) > 0, "No cases found in test_params"

    marker_baud = int((params.get("marker", {}) or {}).get("baudrate", 115200))
    base_timeout_s = float(scenario.get("timeout_s", 1.5))
    dut_port = scenario.get("dut", {}).get("port", "/dev/ttyUSB0")
    default_payload = scenario.get("payload", "Hello VSF\r\n").encode()
    default_baud = int(scenario.get("defaults", {}).get("baudrate", 115200))

    import serial as pyserial
    aux = pyserial.Serial(dut_port, baudrate=marker_baud, timeout=1)

    for c in cases:
        # Scale timeout with data size: 10 bits/byte @ baudrate, factor of 2 margin
        if c.data_size_bytes > 0:
            tx_time_s = (c.data_size_bytes * 10) / default_baud * 2
            timeout_s = max(base_timeout_s, tx_time_s, 5.0)
            payload = _gen_pattern(c.data_size_bytes)
        else:
            timeout_s = base_timeout_s
            payload = default_payload

        serial.expect(f"usart_rx_data:CASE:{c.idx}:READY", timeout=timeout_s)
        aux.write(payload)
        aux.flush()

    serial.expect_test_summary("usart_rx_data", timeout=base_timeout_s)
    aux.close()


def decode(adapter: LogicAnalyzer, channels: dict, capture_path: Path,
           decode_start_ns: int | None = None,
           decode_end_ns: int | None = None, test_params_yml=None) -> None:
    params = load_test_params(test_params_yml)
    scenario = params.get("rx_data", {})
    cases = _parse_cases(scenario)
    pass_cases = [c for c in cases if c.expect_pass]

    marker_baud = int((params.get("marker", {}) or {}).get("baudrate", 115200))
    dut_ch = channels.get(scenario.get("dut", {}).get("channel", "uart1_rx"))
    default_payload = scenario.get("payload", "Hello VSF\r\n").encode()
    out_dir = capture_path.parent

    windows = read_framework_windows(adapter, channels, capture_path, "usart_rx_data",
        decode_start_ns=decode_start_ns, decode_end_ns=decode_end_ns,
    )
    window_by_idx = {w.case_idx: w for w in windows}
    for c in pass_cases:
        assert c.idx in window_by_idx, f"CASE {c.idx}: window missing"

    full_csv = out_dir / f"rx_data_full_{marker_baud}.csv"
    batch_decode_uart(adapter, capture_path, [
        (dut_ch, marker_baud, decode_start_ns, decode_end_ns, full_csv, "none", 8, 1.0)
    ])
    rows = read_csv_rows(full_csv)

    for c in cases:
        if not c.expect_pass:
            print(f"[PASS] CASE {c.idx}  rx_data  expected fail")
            continue
        w = window_by_idx[c.idx]
        got = bytes(b for t, b in rows if w.start_ns <= t < w.end_ns)
        payload = _gen_pattern(c.data_size_bytes) if c.data_size_bytes > 0 else default_payload
        assert got == payload, f"CASE {c.idx}: expected {payload!r}, got {got!r}"
        print(f"[PASS] CASE {c.idx}  rx_data  {got!r}")
