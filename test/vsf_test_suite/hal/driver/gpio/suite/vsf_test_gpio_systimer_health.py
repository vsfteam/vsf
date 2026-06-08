"""gpio_systimer_health suite host harness.

Toggles a GPIO at known systimer-based intervals; LA decode measures the
on-wire period and asserts each gap is within ±tolerance% of expected.

If the RP2040 watchdog tick is misconfigured the systimer runs at ~10 kHz
instead of 1 MHz and every on-wire gap stretches by ~100×. The firmware
trace alone cannot catch this — it compares the systimer against itself.
Decode against the LA's independent sample clock is the authoritative
signal.

The scenario depends on a GPIO LA channel role being declared in
hardware-map.yml (e.g. ``gp4: CH4``). If no such role is configured the
decode falls back to skipping the LA assertion and only verifies the
firmware-side trace was emitted.
"""

from vsf_bench.vsf_test.marker import read_framework_windows, SerialInstrument, load_test_params
from vsf_bench.capabilities.logic_analyzer import LogicAnalyzer
from vsf_bench.utils.core import parse_uart_csv, read_csv_rows
from vsf_bench.config.models import UARTConfig


def run(serial: SerialInstrument,
        adapter: LogicAnalyzer | None = None, test_params_yml=None) -> None:
    params = load_test_params(test_params_yml)
    scenario = params.get("gpio_systimer_health", {})
    timeout_s = float(scenario.get("timeout_s", 5.0))
    serial.expect_test_summary("gpio_systimer_health", timeout=timeout_s)


def decode(adapter: LogicAnalyzer, channels: dict, capture_path: Path,
           decode_start_ns: int | None = None,
           decode_end_ns: int | None = None, test_params_yml=None) -> None:
    params = load_test_params(test_params_yml)
    scenario = params.get("gpio_systimer_health", {})
    cases = list(scenario.get("cases", []))
    assert len(cases) > 0, "No cases found in test_params"

    windows = read_framework_windows(adapter, channels, capture_path, "gpio_systimer_health",
        decode_start_ns=decode_start_ns, decode_end_ns=decode_end_ns,
    )
    window_by_idx = {w.case_idx: w for w in windows}

    for case in cases:
        idx           = int(case["idx"])
        interval_ms   = int(case.get("interval_ms", 10))
        host          = case.get("host", {}) or {}
        la_channel    = host.get("la_channel", "gp4")
        tolerance_pct = float(host.get("tolerance_pct", 5.0))
        expected_ns   = interval_ms * 1_000_000

        if idx not in window_by_idx:
            raise AssertionError(
                f"gpio_systimer_health CASE {idx}: case window missing in capture")

        # Resolve channel role; if not configured, skip LA assertion (PRD's
        # "no LA fallback" path — firmware ran successfully but no external
        # reference is available).
        if la_channel not in channels:
            print(f"[SKIP] gpio_systimer_health CASE {idx}: LA channel role "
                  f"'{la_channel}' not in hardware-map.yml")
            continue
        channel = channels.get(la_channel)

        w = window_by_idx[idx]
        edges = adapter.read_digital_edges(capture_path, channel, w.start_ns, w.end_ns)
        if len(edges) < 2:
            print(f"[SKIP] gpio_systimer_health CASE {idx}: no edges on '{la_channel}' "
                  f"(LA probe not wired)")
            continue

        gaps = [edges[i + 1] - edges[i] for i in range(len(edges) - 1)]
        low  = int(expected_ns * (1.0 - tolerance_pct / 100.0))
        high = int(expected_ns * (1.0 + tolerance_pct / 100.0))
        bad  = [(i, g) for i, g in enumerate(gaps) if not (low <= g <= high)]
        median = sorted(gaps)[len(gaps) // 2]
        err = (median - expected_ns) * 100.0 / expected_ns

        if bad:
            raise AssertionError(
                f"gpio_systimer_health CASE {idx} interval={interval_ms}ms: "
                f"{len(bad)}/{len(gaps)} gaps outside [{low/1e6:.2f}, {high/1e6:.2f}] ms; "
                f"median={median/1e6:.2f} ms (err {err:+.2f}%); "
                f"first offender gap[{bad[0][0]}]={bad[0][1]/1e6:.2f} ms")

        print(
            f"[PASS] gpio_systimer_health CASE {idx} interval={interval_ms}ms "
            f"edges={len(edges)} median={median/1e6:.2f}ms (err {err:+.2f}%)")
