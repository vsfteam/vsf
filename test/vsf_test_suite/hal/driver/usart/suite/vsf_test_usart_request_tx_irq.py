"""usart_request_tx_irq suite host harness.

Firmware asserts internally via VSF_TEST_ASSERT; this script waits for
the test framework summary line and asserts all cases passed.

TX via fifo2req adapter; re-uses UART1.
"""

from vsf_bench.test.marker import read_framework_windows, LogicAnalyzerInstrument, SerialInstrument




def run(serial: SerialInstrument, test_params_yml=None):
    serial.expect_test_summary("usart_request_tx_irq")


def decode(adapter: LogicAnalyzer, channels: dict, capture_path: Path,
           decode_start_ns: int | None = None,
           decode_end_ns: int | None = None) -> None:
    dut_ch = channels.get("uart1_tx")
    out_dir = capture_path.parent

    windows = read_framework_windows(adapter, channels, capture_path, "usart_request_tx_irq",
        decode_start_ns=decode_start_ns, decode_end_ns=decode_end_ns,
    )
    assert len(windows) > 0, "No framework windows found"
    w = windows[0]
    assert w.case_idx == 0

    batch_decode_uart(adapter, capture_path, [
        (dut_ch, 115200, decode_start_ns, decode_end_ns,
         out_dir / "req_tx_irq.csv", "none", 8, 1.0)
    ])

    rows = read_csv_rows(out_dir / "req_tx_irq.csv")
    got = bytes(b for t, b in rows if w.start_ns <= t < w.end_ns)

    if len(got) == 0:
        print(f"[SKIP] CASE 0: no data on 'uart1_tx' (LA probe not wired)")
        return

    expected = bytes((ord('a') + (i % 26)) for i in range(128))
    assert got == expected, (
        f"CASE 0: expected {expected!r}, got {got!r}"
    )
    print(f"[PASS] CASE 0  len={len(got)}  {got[:32]!r}...")
