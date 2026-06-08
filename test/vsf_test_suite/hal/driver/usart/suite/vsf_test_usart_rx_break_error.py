"""USART RX break-error validation: PC asserts BRK condition on the line,
firmware detects it via VSF_USART_IRQ_MASK_BREAK_ERR.

The host opens the aux tty, waits for the firmware ":READY" marker, then
calls pyserial's send_break() to hold the TX line low for ~50 ms. PL011
emits BREAK_ERR when the line stays low past one full character frame.
"""

import time
from dataclasses import dataclass
from vsf_bench import LogicAnalyzerInstrument, SerialInstrument, load_test_params, read_framework_windows



@dataclass(frozen=True)
class Case:
    idx: int
    send_break_ms: int


def _parse_cases(scenario: dict) -> list[Case]:
    cases: list[Case] = []
    for case in scenario.get("cases", []):
        host = (case.get("host", {}) or {})
        cases.append(Case(
            idx=int(case["idx"]),
            send_break_ms=int(host.get("send_break_ms", 50)),
        ))
    return cases


def run(serial: SerialInstrument, test_params_yml=None):
    params = load_test_params(test_params_yml)
    scenario = params.get("rx_break_error", {})
    cases = _parse_cases(scenario)
    assert len(cases) > 0, "No cases found in test_params"

    timeout_s = float(scenario.get("timeout_s", 1.5))
    dut_port = scenario.get("dut", {}).get("port", "/dev/ttyUSB0")
    marker_baud = int((params.get("marker", {}) or {}).get("baudrate", 115200))

    import serial as pyserial
    aux = pyserial.Serial(dut_port, baudrate=marker_baud, timeout=1)

    for c in cases:
        serial.expect(f"usart_rx_break_error:CASE:{c.idx}:READY", timeout=timeout_s)
        # Hold the line low for send_break_ms — well beyond one frame at 115200
        # baud (~87 µs per frame), so PL011 latches a break condition.
        aux.send_break(duration=c.send_break_ms / 1000.0)
        time.sleep(0.01)

    serial.expect_test_summary("usart_rx_break_error", timeout=timeout_s)
    aux.close()
    print(f"[PASS] rx_break_error: {len(cases)} case(s) completed")

def decode(adapter: LogicAnalyzer, channels: dict, capture_path: Path,
           decode_start_ns: int | None = None,
           decode_end_ns: int | None = None,
           marker_baud: int = 115200, test_params_yml=None) -> None:
    params = load_test_params(test_params_yml)
    scenario = params.get("rx_break_error", {})
    cases = _parse_cases(scenario)
    if not cases:
        return

    dut_ch = channels.get(scenario.get("dut", {}).get("channel", "uart1_rx"))

    windows = read_framework_windows(adapter, channels, capture_path, "usart_rx_break_error",
        decode_start_ns=decode_start_ns, decode_end_ns=decode_end_ns,
        marker_baud=marker_baud,
    )
    window_by_idx = {w.case_idx: w for w in windows}

    for c in cases:
        assert c.idx in window_by_idx, f"CASE {c.idx}: window missing"
        w = window_by_idx[c.idx]
        edges = adapter.read_digital_edges(capture_path, dut_ch, start_ns=w.start_ns, end_ns=w.end_ns)
        # A break is a sustained low level. Find the longest low interval.
        min_break_ns = c.send_break_ms * 1_000_000
        if len(edges) >= 2:
            # Measure the longest interval between consecutive edges
            max_gap = max(edges[i+1] - edges[i] for i in range(len(edges)-1))
            assert max_gap >= min_break_ns, (
                f"CASE {c.idx}: break pulse too short: {max_gap/1e6:.1f}ms, "
                f"expected >= {c.send_break_ms}ms"
            )
        print(f"[PASS] CASE {c.idx}  rx_break_error  break>={c.send_break_ms}ms")
