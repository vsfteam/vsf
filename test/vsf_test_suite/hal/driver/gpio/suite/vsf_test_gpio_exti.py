"""gpio_exti suite host harness.

Self-triggered EXTI test: firmware uses SIO output to drive its own pin
and observes the falling edge via EXTI. No external wiring needed.
"""

from vsf_bench import load_test_params, read_framework_windows, LogicAnalyzerInstrument, SerialInstrument


def run(serial: SerialInstrument, test_params_yml=None):
    serial.expect_test_summary("gpio_exti")

def decode(adapter: LogicAnalyzer, channels: dict, capture_path: Path,
           decode_start_ns: int | None = None,
           decode_end_ns: int | None = None,
           marker_baud: int = 115200, test_params_yml=None) -> None:
    params = load_test_params(test_params_yml)
    scenario = params.get("gpio_exti", {})
    cases = scenario.get("cases", [])
    if not cases:
        return

    la_channel = cases[0].get("la_channel", "gp25")
    try:
        ch = channels.get(la_channel)
    except KeyError:
        print(f"[SKIP] gpio_exti decode: no LA channel for '{la_channel}'")
        return

    windows = read_framework_windows(adapter, channels, capture_path, "gpio_exti",
        decode_start_ns=decode_start_ns, decode_end_ns=decode_end_ns,
        marker_baud=marker_baud,
    )
    window_by_idx = {w.case_idx: w for w in windows}

    for case in cases:
        idx = int(case["idx"])
        assert idx in window_by_idx, f"CASE {idx}: window missing"
        w = window_by_idx[idx]
        edges = adapter.read_digital_edges(capture_path, ch, start_ns=w.start_ns, end_ns=w.end_ns)
        assert len(edges) > 0, f"CASE {idx}: no edges detected on {la_channel}"
        print(f"[PASS] CASE {idx}  gpio_exti  edges={len(edges)}")
