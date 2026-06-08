"""USART RX mode validation: PC sends payload at varying parity/data/stop.

Two-phase: `run()` configures the aux serial to each case's mode and writes
payload; `decode()` confirms on-wire bytes match.
"""

from dataclasses import dataclass
from vsf_bench import LogicAnalyzerInstrument, SerialInstrument, load_test_params, read_framework_windows



@dataclass(frozen=True)
class Case:
    idx: int
    expect_pass: bool
    decode_parity: str
    decode_data: int
    decode_stop: float


def _parse_cases(scenario: dict) -> list[Case]:
    cases: list[Case] = []
    for case in scenario.get("cases", []):
        host = (case.get("host", {}) or {}).get("send", {}) or {}
        cases.append(Case(
            idx=int(case["idx"]),
            expect_pass=bool(case.get("expect_pass", True)),
            decode_parity=host.get("parity_type", "none"),
            decode_data=int(host.get("num_data_bits", 8)),
            decode_stop=float(host.get("num_stop_bits", 1.0)),
        ))
    return cases


def run(serial: SerialInstrument, test_params_yml=None):
    params = load_test_params(test_params_yml)
    scenario = params.get("rx_mode", {})
    cases = _parse_cases(scenario)
    assert len(cases) > 0, "No cases found in test_params"

    marker_baud = int((params.get("marker", {}) or {}).get("baudrate", 115200))
    timeout_s = float(scenario.get("timeout_s", 1.5))
    dut_port = scenario.get("dut", {}).get("port", "/dev/ttyUSB0")
    payload = scenario.get("payload", "0123456789\r\n").encode()

    import serial as pyserial
    aux = pyserial.Serial(dut_port, baudrate=marker_baud, timeout=1)
    parity_map = {"none": pyserial.PARITY_NONE, "even": pyserial.PARITY_EVEN, "odd": pyserial.PARITY_ODD}

    for c in cases:
        serial.expect(f"usart_rx_mode:CASE:{c.idx}:READY", timeout=timeout_s)
        aux.parity = parity_map.get(c.decode_parity, pyserial.PARITY_NONE)
        aux.bytesize = c.decode_data
        aux.stopbits = c.decode_stop
        aux.write(payload)
        aux.flush()

    serial.expect_test_summary("usart_rx_mode", timeout=timeout_s)
    aux.close()


def decode(adapter: LogicAnalyzer, channels: dict, capture_path: Path,
           decode_start_ns: int | None = None,
           decode_end_ns: int | None = None, test_params_yml=None) -> None:
    params = load_test_params(test_params_yml)
    scenario = params.get("rx_mode", {})
    cases = _parse_cases(scenario)

    marker_baud = int((params.get("marker", {}) or {}).get("baudrate", 115200))
    dut_ch = channels.get(scenario.get("dut", {}).get("channel", "uart1_rx"))
    payload = scenario.get("payload", "0123456789\r\n").encode()
    out_dir = capture_path.parent

    windows = read_framework_windows(adapter, channels, capture_path, "usart_rx_mode",
        decode_start_ns=decode_start_ns, decode_end_ns=decode_end_ns,
    )
    window_by_idx = {w.case_idx: w for w in windows}
    for c in cases:
        assert c.idx in window_by_idx, f"CASE {c.idx}: window missing"

    unique_configs = sorted({(c.decode_parity, c.decode_data, c.decode_stop) for c in cases})
    config_to_csv = {
        cfg: out_dir / f"rx_mode_full_{cfg[0]}_{cfg[1]}_{cfg[2]}.csv"
        for cfg in unique_configs
    }
    batch_decode_uart(adapter, capture_path, [
        (dut_ch, marker_baud, decode_start_ns, decode_end_ns,
         config_to_csv[(p, d, s)], p, d, s)
        for (p, d, s) in unique_configs
    ])

    for c in cases:
        w = window_by_idx[c.idx]
        rows = read_csv_rows(config_to_csv[(c.decode_parity, c.decode_data, c.decode_stop)])
        got = bytes(b for t, b in rows if w.start_ns <= t < w.end_ns)
        # 5-bit/6-bit/7-bit: on-wire bytes are truncated to N LSBs.
        if c.decode_data < 8:
            mask = (1 << c.decode_data) - 1
            expected = bytes(b & mask for b in payload)
        else:
            expected = payload
        assert got == expected, (
            f"CASE {c.idx} mode={c.decode_parity}/{c.decode_data}/{c.decode_stop}: "
            f"expected {expected!r}, got {got!r}"
        )
        print(f"[PASS] CASE {c.idx}  mode={c.decode_parity}/{c.decode_data}/{c.decode_stop}  {got!r}")
