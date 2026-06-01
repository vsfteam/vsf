"""usart_rx_fifo_irq suite host harness.

Host sends incrementing-counter payload via aux_serial after each READY marker.
Firmware receives via RX IRQ handler and asserts every byte matches.

Requires the aux serial fixture: host drives /dev/ttyUSB0 -> Pico UART1 RX.
"""


from vsf_bench import SerialInstrument, load_test_params


def _gen_pattern(size: int) -> bytes:
    """Incrementing-counter pattern: byte[i] = i & 0xFF."""
    return bytes(i & 0xFF for i in range(size))


def run(serial: SerialInstrument, test_params_yml=None):
    params = load_test_params(test_params_yml)
    scenario = params.get("rx_fifo_irq", {})
    cases = scenario.get("cases", [])
    if not cases:
        return

    timeout_s = float(scenario.get("timeout_s", 10.0))
    dut_port = scenario.get("dut", {}).get("port", "/dev/ttyUSB0")
    marker_baud = int((params.get("marker", {}) or {}).get("baudrate", 115200))
    # PL011 RX FIFO depth is 32 bytes; total = depth * refill_target
    fifo_depth = int(scenario.get("fifo_depth", 32))

    import serial as pyserial
    aux = pyserial.Serial(dut_port, baudrate=marker_baud, timeout=1)

    for case in cases:
        idx = int(case["idx"])
        refill_target = int(case.get("refill_target", 4))
        total = fifo_depth * refill_target
        serial.expect(f"usart_rx_fifo_irq:CASE:{idx}:READY", timeout=timeout_s)
        payload = _gen_pattern(total)
        aux.write(payload)
        aux.flush()

    serial.expect_test_summary("usart_rx_fifo_irq", timeout=timeout_s)
    aux.close()
    print(f"[PASS] rx_fifo_irq: {len(cases)} case(s) completed")
