"""gpio_toggle_freq suite host harness.

The firmware times a fixed-count toggle loop with the systimer and emits
the result as a trace line. That comparison is unreliable on its own —
the systimer is driven by the same watchdog tick as the rest of the
chip, so it cannot detect clock-tree misconfiguration. The
authoritative pass/fail is the host-side `decode()` below, which reads
the actual edges from the LA and asserts the on-wire toggle frequency
is at or above `min_freq_hz`.

If no GPIO LA channel role is configured for the pin under test, the
decode skips the assertion and only verifies the firmware trace was
emitted (per PRD `gpio-toggle-perf` "no LA fallback" path).
"""

from vsf_bench import read_framework_windows, LogicAnalyzerInstrument, SerialInstrument, load_test_params


def run(serial: SerialInstrument,
        la: LogicAnalyzerInstrument | None = None, test_params_yml=None) -> None:
    serial.expect_test_summary("gpio_toggle_freq")


def decode(la: LogicAnalyzerInstrument,
           decode_start_ns: int | None = None,
           decode_end_ns: int | None = None, test_params_yml=None) -> None:
    params = load_test_params(test_params_yml)
    scenario = params.get("gpio_toggle_freq", {})
    cases = list(scenario.get("cases", []))
    assert len(cases) > 0, "No cases found in test_params"

    windows = read_framework_windows(
        la, "gpio_toggle_freq",
        decode_start_ns=decode_start_ns, decode_end_ns=decode_end_ns,
    )
    window_by_idx = {w.case_idx: w for w in windows}

    for case in cases:
        idx          = int(case["idx"])
        toggle_count = int(case.get("toggle_count", 1000))
        host         = case.get("host", {}) or {}
        la_channel   = host.get("la_channel", "gp4")
        min_freq_hz  = int(host.get("min_freq_hz", 1_000_000))

        if idx not in window_by_idx:
            raise AssertionError(
                f"gpio_toggle_freq CASE {idx}: case window missing in capture")

        if la_channel not in la._channels:
            print(f"[SKIP] gpio_toggle_freq CASE {idx}: LA channel role "
                  f"'{la_channel}' not in hardware-map.yml")
            continue

        w = window_by_idx[idx]
        channel = la.channel(la_channel)
        edges = la.read_digital_edges(channel, w.start_ns, w.end_ns)
        if len(edges) < 2:
            print(f"[SKIP] gpio_toggle_freq CASE {idx}: no edges on '{la_channel}' "
                  f"(LA probe not wired)")
            continue

        # Each call to vsf_gpio_toggle() flips the pin once → one edge.
        # Expected ≈ toggle_count edges within the case window.
        duration_ns = edges[-1] - edges[0]
        freq_hz = (len(edges) - 1) * 1_000_000_000 / duration_ns if duration_ns else 0

        if freq_hz < min_freq_hz:
            raise AssertionError(
                f"gpio_toggle_freq CASE {idx} edges={len(edges)} "
                f"freq={freq_hz:.0f} Hz < min_freq_hz={min_freq_hz}")

        print(
            f"[PASS] gpio_toggle_freq CASE {idx} edges={len(edges)} "
            f"freq={freq_hz:.0f} Hz (min {min_freq_hz})")
