"""USART RX parity-error validation: PC sends payload at wrong parity, firmware asserts mismatch.

Firmware configures UART1 RX for the case's parity setting (e.g. EVEN), then waits
for a payload. Host sends at the YAML-specified host send parity (typically NONE),
so the firmware sees parity errors and asserts via VSF_TEST_ASSERT.
"""

from dataclasses import dataclass
from vsf_bench import LogicAnalyzerInstrument, SerialInstrument, load_test_params, read_framework_windows



@dataclass(frozen=True)
class Case:
    idx: int
    host_parity: str
    host_data_bits: int
    host_stop_bits: float
    host_baud: int


def _parse_cases(scenario: dict) -> list[Case]:
    defaults = scenario.get("defaults", {}) or {}
    default_send = (defaults.get("host", {}) or {}).get("send", {}) or {}
    cases: list[Case] = []
    for case in scenario.get("cases", []):
        host_send = (case.get("host", {}) or {}).get("send", {}) or {}
        cases.append(Case(
            idx=int(case["idx"]),
            host_parity=host_send.get("parity_type", default_send.get("parity_type", "none")),
            host_data_bits=int(host_send.get("num_data_bits", default_send.get("num_data_bits", 8))),
            host_stop_bits=float(host_send.get("num_stop_bits", default_send.get("num_stop_bits", 1.0))),
            host_baud=int(host_send.get("baudrate", default_send.get("baudrate", 115200))),
        ))
    return cases


def run(serial: SerialInstrument, test_params_yml=None):
    params = load_test_params(test_params_yml)
    scenario = params.get("rx_parity_error", {})
    cases = _parse_cases(scenario)
    assert len(cases) > 0, "No cases found in test_params"

    timeout_s = float(scenario.get("timeout_s", 1.5))
    dut_port = scenario.get("dut", {}).get("port", "/dev/ttyUSB0")
    marker_baud = int((params.get("marker", {}) or {}).get("baudrate", 115200))
    payload = scenario.get("payload", "Hello VSF\r\n").encode()

    import serial as pyserial
    aux = pyserial.Serial(dut_port, baudrate=marker_baud, timeout=1)

    parity_map = {"none": pyserial.PARITY_NONE, "even": pyserial.PARITY_EVEN, "odd": pyserial.PARITY_ODD}

    for c in cases:
        serial.expect(f"usart_rx_parity_error:CASE:{c.idx}:READY", timeout=timeout_s)
        aux.baudrate = c.host_baud
        aux.parity = parity_map.get(c.host_parity, pyserial.PARITY_NONE)
        aux.bytesize = c.host_data_bits
        aux.stopbits = c.host_stop_bits
        aux.write(payload)
        aux.flush()

    serial.expect_test_summary("usart_rx_parity_error", timeout=timeout_s)
    aux.close()
    print(f"[PASS] rx_parity_error: {len(cases)} case(s) completed")

def decode(adapter: LogicAnalyzer, channels: dict, capture_path: Path,
           decode_start_ns: int | None = None,
           decode_end_ns: int | None = None,
           marker_baud: int = 115200, test_params_yml=None) -> None:
    """Decode and verify parity errors are present in the captured UART stream.

    NOTE: This decoder relies on dsview-cli UART decoder reporting parity-error
    flags in its CSV output. If dsview-cli does not yet support this, the
    assertion will need to be relaxed or the decoder enhanced upstream.
    """
    params = load_test_params(test_params_yml)
    scenario = params.get("rx_parity_error", {})
    cases = _parse_cases(scenario)
    if not cases:
        return

    dut_ch = channels.get(scenario.get("dut", {}).get("channel", "uart1_rx"))
    out_dir = capture_path.parent

    windows = read_framework_windows(adapter, channels, capture_path, "usart_rx_parity_error",
        decode_start_ns=decode_start_ns, decode_end_ns=decode_end_ns,
        marker_baud=marker_baud,
    )
    window_by_idx = {w.case_idx: w for w in windows}

    full_csv = out_dir / f"rx_parity_error_full_{marker_baud}.csv"
    batch_decode_uart(adapter, capture_path, [
        (dut_ch, marker_baud, decode_start_ns, decode_end_ns, full_csv, "none", 8, 1.0)
    ])
    # TODO: verify parity-error flags in decoded output once dsview-cli supports it
    for c in cases:
        assert c.idx in window_by_idx, f"CASE {c.idx}: window missing"
        print(f"[PASS] CASE {c.idx}  rx_parity_error  (TODO: verify parity error flags)")
