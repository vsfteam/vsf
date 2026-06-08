"""Multi-mode UART TX validation via logic analyzer capture.

Two-phase: `run()` waits for firmware completion; `decode()` walks the shared
LA capture and validates each case's UART frame at its specific parity / data /
stop configuration.
"""

from dataclasses import dataclass
from vsf_bench.test.marker import read_framework_windows, SerialInstrument, load_test_params
from vsf_bench.capabilities.logic_analyzer import LogicAnalyzer
from vsf_bench.utils.core import batch_decode_uart, parse_uart_csv, read_csv_rows
from vsf_bench.config import UARTConfig



@dataclass(frozen=True)
class Case:
    idx: int
    baud: int
    expect_pass: bool
    decode_parity: str
    decode_data: int
    decode_stop: float


def _parse_cases(scenario: dict) -> list[Case]:
    defaults = scenario.get("defaults", {}) or {}
    default_baud = int(defaults.get("baudrate", 115200))
    default_expect = bool(defaults.get("expect_pass", True))

    cases: list[Case] = []
    for case in scenario.get("cases", []):
        la = case.get("la", {}) or {}
        decode = la.get("decode", {}) or {}
        host = case.get("host", {}) or {}
        send = host.get("send", {}) or {}
        cases.append(Case(
            idx=int(case["idx"]),
            baud=int(send.get("baudrate", default_baud)),
            expect_pass=bool(case.get("expect_pass", default_expect)),
            decode_parity=decode.get("parity_type", "none"),
            decode_data=int(decode.get("num_data_bits", 8)),
            decode_stop=float(decode.get("num_stop_bits", 1.0)),
        ))
    return cases


def run(serial: SerialInstrument, test_params_yml=None):
    params = load_test_params(test_params_yml)
    scenario = params.get("tx_mode", {})
    timeout_s = float(scenario.get("timeout_s", 1.5))
    serial.expect_test_summary("usart_mode", timeout=timeout_s)


def decode(adapter: LogicAnalyzer, channels: dict, capture_path: Path,
           decode_start_ns: int | None = None,
           decode_end_ns: int | None = None, test_params_yml=None) -> None:
    params = load_test_params(test_params_yml)
    scenario = params.get("tx_mode", {})
    cases = _parse_cases(scenario)
    assert len(cases) > 0, "No cases found in test_params"

    dut_ch = channels.get(scenario.get("dut", {}).get("channel", "uart1_tx"))
    payload = scenario.get("payload", "Hello VSF\r\n").encode()
    out_dir = capture_path.parent

    windows = read_framework_windows(adapter, channels, capture_path, "usart_mode",
        decode_start_ns=decode_start_ns, decode_end_ns=decode_end_ns,
    )
    window_by_idx = {w.case_idx: w for w in windows}
    for c in cases:
        assert c.idx in window_by_idx, f"CASE {c.idx}: window missing"

    unique_configs = sorted({(c.baud, c.decode_parity, c.decode_data, c.decode_stop) for c in cases})
    config_to_csv = {
        cfg: out_dir / f"mode_full_{cfg[1]}_{cfg[2]}_{cfg[3]}.csv"
        for cfg in unique_configs
    }
    batch_decode_uart(adapter, capture_path, [
        (dut_ch, baud, decode_start_ns, decode_end_ns,
         config_to_csv[(baud, par, data, stop)], par, data, stop)
        for (baud, par, data, stop) in unique_configs
    ])

    for c in cases:
        w = window_by_idx[c.idx]
        rows = read_csv_rows(config_to_csv[(c.baud, c.decode_parity, c.decode_data, c.decode_stop)])
        got = bytes(b for t, b in rows if w.start_ns <= t < w.end_ns)
        # Sub-byte data lengths transmit only the LSBs — the decoder reports
        # the lowest `decode_data` bits. Mask the reference payload to match.
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
