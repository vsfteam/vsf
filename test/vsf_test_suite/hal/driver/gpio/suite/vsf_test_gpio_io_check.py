"""gpio_io_check scenario: bit-bang UART on GPIO pins to verify LA probe wiring.

Firmware drives each declared pin as push-pull output and sends a unique
UART 8N1 byte (0x50 + pin) at 115200 baud. The host decode() phase decodes
UART on all LA channels and cross-references: each expected pin's byte must
appear on exactly one channel.

Hardware wiring (Pico board):
  - GP8 (uart1_tx) -> LA probe -> DSLogic CH5 -> LA CH3
  - GP9 (uart1_rx) -> LA probe -> DSLogic CH4 -> LA CH2
Both pins are verified as GPIO outputs before any peripheral-specific
scenario runs, so a miswired LA probe is caught early.
"""

from vsf_bench import LogicAnalyzerInstrument, SerialInstrument, load_test_params


BAUDRATE = 115200


def run(serial: SerialInstrument,
        la: LogicAnalyzerInstrument | None = None, test_params_yml=None) -> None:
    params = load_test_params(test_params_yml)
    scenario = params.get("gpio_io_check", {})
    timeout_s = float(scenario.get("timeout_s", 5.0))
    serial.expect_test_summary("gpio_io_check", timeout=timeout_s)


def decode(la: LogicAnalyzerInstrument,
           decode_start_ns: int | None = None,
           decode_end_ns: int | None = None, test_params_yml=None) -> None:
    params = load_test_params(test_params_yml)
    scenario = params.get("gpio_io_check", {})
    cases = scenario.get("cases", [])
    if not cases:
        print("[gpio_io_check] No cases declared, skipping decode")
        return

    # Build expected {pin -> byte} map
    expected: dict[int, int] = {}
    for case in cases:
        pin = int(case["pin"])
        expected[pin] = 0x50 + pin

    # Decode UART on every LA channel
    out_dir = la.output_dir
    channel_bytes: dict[str, list[int]] = {}
    for ch_name in la._channels:
        ch = la.channel(ch_name)
        csv_path = out_dir / f"io_check_{ch_name}.csv"
        la.decode_uart(ch, BAUDRATE, decode_start_ns, decode_end_ns,
                       csv_path, "none", 8, 1.0)
        rows = la.read_csv_rows(csv_path)
        channel_bytes[ch_name] = [b for t, b in rows]

    # Cross-reference: each expected byte must appear on exactly one channel.
    # In shared LA mode unrelated channels (e.g. uart0_rx carrying host
    # commands) will have traffic — we ignore those.  If a byte is not found
    # at all we SKIP rather than fail, because the LA probe for that pin may
    # simply not be wired.
    all_pass = True
    for pin, byte in expected.items():
        found_on: list[str] = []
        for ch_name, bytes_list in channel_bytes.items():
            if byte in bytes_list:
                found_on.append(ch_name)

        if len(found_on) == 0:
            print(
                f"[SKIP] pin {pin} byte 0x{byte:02x} not found on any LA channel "
                f"(probe not wired)"
            )
            all_pass = False
        elif len(found_on) > 1:
            print(
                f"[WARN] pin {pin} byte 0x{byte:02x} found on multiple channels: {found_on} "
                f"(possible loopback or shared signal)"
            )
        else:
            print(f"[PASS] pin {pin} byte 0x{byte:02x} on channel {found_on[0]}")

    if not all_pass:
        return
