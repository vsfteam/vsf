"""USART RX overflow-error validation: PC blasts a burst that exceeds the
PL011 32-byte FIFO; firmware detects it via VSF_USART_IRQ_MASK_RX_OVERFLOW_ERR.

The firmware leaves the RX FIFO un-drained (only the OVERFLOW IRQ mask is
enabled), so a host write of 128 bytes at 115200 baud quickly overruns the
chip. The OE flag latches and the ISR sets ctx.overflow_err.
"""

import time
from dataclasses import dataclass
from vsf_bench import LogicAnalyzerInstrument, SerialInstrument, load_test_params, read_framework_windows



@dataclass(frozen=True)
class Case:
    idx: int
    burst_size: int


def _parse_cases(scenario: dict) -> list[Case]:
    cases: list[Case] = []
    for case in scenario.get("cases", []):
        host = (case.get("host", {}) or {})
        cases.append(Case(
            idx=int(case["idx"]),
            burst_size=int(host.get("burst_size", 128)),
        ))
    return cases


def run(serial: SerialInstrument, test_params_yml=None):
    params = load_test_params(test_params_yml)
    scenario = params.get("rx_overflow_error", {})
    cases = _parse_cases(scenario)
    assert len(cases) > 0, "No cases found in test_params"

    timeout_s = float(scenario.get("timeout_s", 1.5))
    dut_port = scenario.get("dut", {}).get("port", "/dev/ttyUSB0")
    marker_baud = int((params.get("marker", {}) or {}).get("baudrate", 115200))

    import serial as pyserial
    aux = pyserial.Serial(dut_port, baudrate=marker_baud, timeout=1)

    for c in cases:
        serial.expect(f"usart_rx_overflow_error:CASE:{c.idx}:READY", timeout=timeout_s)
        # PL011 RX FIFO is 32 bytes; 128+ bytes is plenty.
        aux.write(bytes([0xAA] * c.burst_size))
        aux.flush()
        time.sleep(0.05)

    serial.expect_test_summary("usart_rx_overflow_error", timeout=timeout_s)
    aux.close()
    print(f"[PASS] rx_overflow_error: {len(cases)} case(s) completed")

def decode(adapter: LogicAnalyzer, channels: dict, capture_path: Path,
           decode_start_ns: int | None = None,
           decode_end_ns: int | None = None,
           marker_baud: int = 115200, test_params_yml=None) -> None:
    params = load_test_params(test_params_yml)
    scenario = params.get("rx_overflow_error", {})
    cases = _parse_cases(scenario)
    if not cases:
        return

    dut_ch = channels.get(scenario.get("dut", {}).get("channel", "uart1_rx"))
    out_dir = capture_path.parent

    windows = read_framework_windows(adapter, channels, capture_path, "usart_rx_overflow_error",
        decode_start_ns=decode_start_ns, decode_end_ns=decode_end_ns,
        marker_baud=marker_baud,
    )
    window_by_idx = {w.case_idx: w for w in windows}

    full_csv = out_dir / f"rx_overflow_error_full_{marker_baud}.csv"
    batch_decode_uart(adapter, capture_path, [
        (dut_ch, marker_baud, decode_start_ns, decode_end_ns, full_csv, "none", 8, 1.0)
    ])
    rows = read_csv_rows(full_csv)

    for c in cases:
        assert c.idx in window_by_idx, f"CASE {c.idx}: window missing"
        w = window_by_idx[c.idx]
        got = bytes(b for t, b in rows if w.start_ns <= t < w.end_ns)
        # PL011 FIFO is 32 bytes; verify the burst exceeds FIFO depth
        assert len(got) >= 32, (
            f"CASE {c.idx}: expected >=32 bytes on wire, got {len(got)}"
        )
        print(f"[PASS] CASE {c.idx}  rx_overflow_error  wire_bytes={len(got)}")
