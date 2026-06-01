"""usart_rx_bulk_irq scenario: RX IRQ-driven sustained bulk transfer.

Host sends a large incrementing-counter payload via aux UART. Firmware
receives via RX IRQ handler and asserts every byte matches.

Requires the aux serial fixture: host drives /dev/ttyUSB0 → Pico UART1 RX.
"""


from vsf_bench import read_framework_windows, LogicAnalyzerInstrument, SerialInstrument, load_test_params




def _gen_pattern(size: int) -> bytes:
    """Incrementing-counter pattern: byte[i] = i & 0xFF."""
    return bytes(i & 0xFF for i in range(size))


def run(serial: SerialInstrument, test_params_yml=None):
    params = load_test_params(test_params_yml)
    scenario = params.get("rx_bulk_irq", {})
    cases = scenario.get("cases", [])
    if not cases:
        return

    timeout_s = float(scenario.get("timeout_s", 30.0))
    suite_timeout_s = float(scenario.get("suite_timeout_s", 0.0))
    dut_port = scenario.get("dut", {}).get("port", "/dev/ttyUSB0")
    marker_baud = int((params.get("marker", {}) or {}).get("baudrate", 115200))

    import serial as pyserial
    aux = pyserial.Serial(dut_port, baudrate=marker_baud, timeout=1)

    # Track suite-level deadline if configured
    suite_deadline = None
    if suite_timeout_s > 0:
        import time
        suite_deadline = time.monotonic() + suite_timeout_s

    for case in cases:
        idx = int(case["idx"])
        sz = int(case.get("data_size_bytes", 1024))
        serial.expect(f"usart_rx_bulk_irq:CASE:{idx}:READY", timeout=timeout_s)
        payload = _gen_pattern(sz)
        aux.write(payload)
        aux.flush()

    serial.expect_test_summary("usart_rx_bulk_irq", timeout=timeout_s)

    if suite_deadline is not None and time.monotonic() > suite_deadline:
        from vsf_bench import SuiteTimeoutError
        raise SuiteTimeoutError(
            f"usart_rx_bulk_irq exceeded suite_timeout_s={suite_timeout_s}s"
        )

    aux.close()

def decode(la: LogicAnalyzerInstrument,
           decode_start_ns: int | None = None,
           decode_end_ns: int | None = None,
           marker_baud: int = 115200, test_params_yml=None) -> None:
    params = load_test_params(test_params_yml)
    scenario = params.get("rx_bulk_irq", {})
    cases = scenario.get("cases", [])
    if not cases:
        return

    dut_ch = la.channel(scenario.get("dut", {}).get("channel", "uart1_rx"))
    out_dir = la.output_dir

    windows = read_framework_windows(
        la, "usart_rx_bulk_irq",
        decode_start_ns=decode_start_ns, decode_end_ns=decode_end_ns,
        marker_baud=marker_baud,
    )
    window_by_idx = {w.case_idx: w for w in windows}

    full_csv = out_dir / f"rx_bulk_irq_full_{marker_baud}.csv"
    la.batch_decode_uart([
        (dut_ch, marker_baud, decode_start_ns, decode_end_ns, full_csv, "none", 8, 1.0)
    ])
    rows = la.read_csv_rows(full_csv)

    for case in cases:
        idx = int(case["idx"])
        sz = int(case.get("data_size_bytes", 1024))
        payload = _gen_pattern(sz)
        assert idx in window_by_idx, f"CASE {idx}: window missing"
        w = window_by_idx[idx]
        got = bytes(b for t, b in rows if w.start_ns <= t < w.end_ns)
        assert got == payload, f"CASE {idx}: expected {payload!r}, got {got!r}"
        print(f"[PASS] CASE {idx}  rx_bulk_irq  sz={sz}")
