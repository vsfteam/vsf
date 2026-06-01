"""gpio_atomic suite host harness.

Firmware asserts internally via VSF_TEST_ASSERT; this script waits for
the test framework summary line and asserts all cases passed.

Requires GP4/GP5 jumper (shared mode).
"""

from vsf_bench import load_test_params, read_framework_windows, LogicAnalyzerInstrument, SerialInstrument


def run(serial: SerialInstrument, test_params_yml=None):
    serial.expect_test_summary("gpio_atomic")

def decode(la: LogicAnalyzerInstrument,
           decode_start_ns: int | None = None,
           decode_end_ns: int | None = None,
           marker_baud: int = 115200, test_params_yml=None) -> None:
    params = load_test_params(test_params_yml)
    scenario = params.get("gpio_atomic", {})
    cases = scenario.get("cases", [])
    if not cases:
        return

    la_channel = cases[0].get("la_channel", "gp25")
    try:
        ch = la.channel(la_channel)
    except KeyError:
        print(f"[SKIP] gpio_atomic decode: no LA channel for '{la_channel}'")
        return

    windows = read_framework_windows(
        la, "gpio_atomic",
        decode_start_ns=decode_start_ns, decode_end_ns=decode_end_ns,
        marker_baud=marker_baud,
    )
    window_by_idx = {w.case_idx: w for w in windows}

    for case in cases:
        idx = int(case["idx"])
        assert idx in window_by_idx, f"CASE {idx}: window missing"
        w = window_by_idx[idx]
        edges = la.read_digital_edges(ch, start_ns=w.start_ns, end_ns=w.end_ns)
        assert len(edges) > 0, f"CASE {idx}: no edges detected on {la_channel}"
        print(f"[PASS] CASE {idx}  gpio_atomic  edges={len(edges)}")
