"""usart_rx_fifo_threshold suite host harness.

Host sends an exact number of bytes via aux UART to trigger the RX FIFO
threshold IRQ. Firmware asserts the IRQ fired when the expected byte count
was reached.

Requires the aux serial fixture: host drives /dev/ttyUSB0 -> Pico UART1 RX.
"""


from vsf_bench import SerialInstrument, load_test_params




def _gen_pattern(size: int) -> bytes:
    """Incrementing-counter pattern: byte[i] = i & 0xFF."""
    return bytes(i & 0xFF for i in range(size))


def run(serial: SerialInstrument, test_params_yml=None):
    params = load_test_params(test_params_yml)
    scenario = params.get("rx_fifo_threshold", {})
    cases = scenario.get("cases", [])
    if not cases:
        return

    timeout_s = float(scenario.get("timeout_s", 10.0))
    dut_port = scenario.get("dut", {}).get("port", "/dev/ttyUSB0")
    marker_baud = int((params.get("marker", {}) or {}).get("baudrate", 115200))

    import serial as pyserial
    aux = pyserial.Serial(dut_port, baudrate=marker_baud, timeout=1)

    for case in cases:
        idx = int(case["idx"])
        sz = int(case.get("expected_bytes", 16))
        serial.expect(f"usart_rx_fifo_threshold:CASE:{idx}:READY", timeout=timeout_s)
        payload = _gen_pattern(sz)
        aux.write(payload)
        aux.flush()

    serial.expect_test_summary("usart_rx_fifo_threshold", timeout=timeout_s)

    aux.close()
