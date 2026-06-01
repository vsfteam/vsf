"""USART RX timeout validation: PC sends partial data, firmware detects timeout.

Pure handshake — no LA decode needed because the firmware asserts on the
timeout condition internally.
"""

from dataclasses import dataclass
from vsf_bench import LogicAnalyzerInstrument, SerialInstrument, load_test_params, read_framework_windows



@dataclass(frozen=True)
class Case:
    idx: int
    expect_pass: bool


def _parse_cases(scenario: dict) -> list[Case]:
    cases: list[Case] = []
    for case in scenario.get("cases", []):
        cases.append(Case(
            idx=int(case["idx"]),
            expect_pass=bool(case.get("expect_pass", True)),
        ))
    return cases


def run(serial: SerialInstrument, test_params_yml=None):
    params = load_test_params(test_params_yml)
    scenario = params.get("rx_timeout", {})
    cases = _parse_cases(scenario)
    assert len(cases) > 0, "No cases found in test_params"

    marker_baud = int((params.get("marker", {}) or {}).get("baudrate", 115200))
    timeout_s = float(scenario.get("timeout_s", 1.5))
    dut_port = scenario.get("dut", {}).get("port", "/dev/ttyUSB0")

    import serial as pyserial
    aux = pyserial.Serial(dut_port, baudrate=marker_baud, timeout=1)

    for c in cases:
        serial.expect(f"usart_rx_timeout:CASE:{c.idx}:READY", timeout=timeout_s)
        # Send only 2 bytes (partial data) — firmware should timeout.
        aux.write(b"AB")
        aux.flush()

    serial.expect_test_summary("usart_rx_timeout", timeout=timeout_s)
    aux.close()
    print(f"[PASS] rx_timeout: {len(cases)} case(s) completed")

def decode(la: LogicAnalyzerInstrument,
           decode_start_ns: int | None = None,
           decode_end_ns: int | None = None,
           marker_baud: int = 115200, test_params_yml=None) -> None:
    params = load_test_params(test_params_yml)
    scenario = params.get("rx_timeout", {})
    cases = _parse_cases(scenario)
    pass_cases = [c for c in cases if c.expect_pass]
    if not pass_cases:
        return

    dut_ch = la.channel(scenario.get("dut", {}).get("channel", "uart1_rx"))
    out_dir = la.output_dir

    windows = read_framework_windows(
        la, "usart_rx_timeout",
        decode_start_ns=decode_start_ns, decode_end_ns=decode_end_ns,
        marker_baud=marker_baud,
    )
    window_by_idx = {w.case_idx: w for w in windows}
    for c in pass_cases:
        assert c.idx in window_by_idx, f"CASE {c.idx}: window missing"

    full_csv = out_dir / f"rx_timeout_full_{marker_baud}.csv"
    la.batch_decode_uart([
        (dut_ch, marker_baud, decode_start_ns, decode_end_ns, full_csv, "none", 8, 1.0)
    ])
    rows = la.read_csv_rows(full_csv)

    for c in pass_cases:
        w = window_by_idx[c.idx]
        got = bytes(b for t, b in rows if w.start_ns <= t < w.end_ns)
        # Firmware only receives 2 bytes ("AB"); verify no more than that
        assert len(got) <= 2, f"CASE {c.idx}: expected <=2 bytes, got {len(got)} bytes {got!r}"
        print(f"[PASS] CASE {c.idx}  rx_timeout  received {len(got)} bytes")
