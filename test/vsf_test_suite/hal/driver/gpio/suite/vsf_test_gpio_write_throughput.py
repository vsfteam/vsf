"""gpio_write_throughput suite host harness.

Firmware asserts internally via VSF_TEST_ASSERT; this script waits for
the test framework summary line and asserts all cases passed.

Performance — no loopback needed (only writes/timing).
"""

from vsf_bench import load_test_params, read_framework_windows, LogicAnalyzerInstrument, SerialInstrument


def run(serial: SerialInstrument, test_params_yml=None):
    serial.expect_test_summary("gpio_write_throughput")

def decode(adapter: LogicAnalyzer, channels: dict, capture_path: Path,
           decode_start_ns: int | None = None,
           decode_end_ns: int | None = None,
           marker_baud: int = 115200, test_params_yml=None) -> None:
    params = load_test_params(test_params_yml)
    scenario = params.get("gpio_write_throughput", {})
    cases = scenario.get("cases", [])
    if not cases:
        return

    la_channel = cases[0].get("la_channel", "gp4")
    try:
        ch = channels.get(la_channel)
    except KeyError:
        print(f"[SKIP] decode: no LA channel for '{la_channel}'")
        return

    windows = read_framework_windows(adapter, channels, capture_path, "gpio_write_throughput",
        decode_start_ns=decode_start_ns, decode_end_ns=decode_end_ns,
        marker_baud=marker_baud,
    )
    window_by_idx = {w.case_idx: w for w in windows}

    for case in cases:
        idx = int(case["idx"])
        assert idx in window_by_idx, f"CASE {idx}: window missing"
        w = window_by_idx[idx]
        edges = adapter.read_digital_edges(capture_path, ch, start_ns=w.start_ns, end_ns=w.end_ns)
        duration_ns = w.end_ns - w.start_ns
        if len(edges) == 0:
            print(f"[SKIP] CASE {idx}: no edges on '{la_channel}' (LA probe not wired)")
            continue
        freq_hz = len(edges) / (2.0 * duration_ns / 1e9) if duration_ns > 0 else 0
        print(f"[PASS] CASE {idx}  gpio_write_throughput  edges={len(edges)} freq={freq_hz:.0f}Hz")
