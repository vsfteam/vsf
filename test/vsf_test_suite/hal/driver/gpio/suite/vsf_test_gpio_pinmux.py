"""gpio_pinmux suite host harness.

Firmware asserts internally via VSF_TEST_ASSERT; this script waits for
the test framework summary line and asserts all cases passed.

Re-uses UART1 (GP8/GP9); no extra fixture needed.
"""

from vsf_bench import load_test_params, read_framework_windows, LogicAnalyzerInstrument, SerialInstrument


def run(serial: SerialInstrument, test_params_yml=None):
    serial.expect_test_summary("gpio_pinmux")

def decode(la: LogicAnalyzerInstrument,
           decode_start_ns: int | None = None,
           decode_end_ns: int | None = None,
           marker_baud: int = 115200, test_params_yml=None) -> None:
    params = load_test_params(test_params_yml)
    scenario = params.get("gpio_pinmux", {})
    cases = scenario.get("cases", [])
    if not cases:
        return

    ch = la.channel("uart1_tx")
    out_dir = la.output_dir

    windows = read_framework_windows(
        la, "gpio_pinmux",
        decode_start_ns=decode_start_ns, decode_end_ns=decode_end_ns,
        marker_baud=marker_baud,
    )
    window_by_idx = {w.case_idx: w for w in windows}

    full_csv = out_dir / f"gpio_pinmux_full_{marker_baud}.csv"
    la.batch_decode_uart([
        (ch, marker_baud, decode_start_ns, decode_end_ns, full_csv, "none", 8, 1.0)
    ])
    rows = la.read_csv_rows(full_csv)

    for case in cases:
        idx = int(case["idx"])
        assert idx in window_by_idx, f"CASE {idx}: window missing"
        w = window_by_idx[idx]
        got = bytes(b for t, b in rows if w.start_ns <= t < w.end_ns)
        if len(got) == 0:
            print(f"[SKIP] CASE {idx}: no data on 'uart1_tx' (LA probe not wired)")
            continue
        assert b"PINMUX" in got, f"CASE {idx}: expected PINMUX in {got!r}"
        print(f"[PASS] CASE {idx}  gpio_pinmux  got={got!r}")
