"""arch_systimer_freq scenario: verify systimer frequency via serial pulse markers.

Firmware prints a marker, busy-waits a known number of systimer ticks, then prints
a second marker.  The host timestamps the wall-clock arrival of each marker via
``time.monotonic()`` and computes the independent frequency::

    measured_freq = expected_ticks / (t1 - t0)

If the clock tree feeding the systimer is misconfigured, the host-measured
marker gap will reveal the actual tick rate independently of what the
firmware reports — the UART baud rate is independent of the systimer
clock, so the host's wall clock is an authoritative reference.
"""

import time

from vsf_bench import SerialInstrument, LogicAnalyzerInstrument, load_test_params


def run(serial: SerialInstrument,
        la: LogicAnalyzerInstrument | None = None, test_params_yml=None) -> None:
    params = load_test_params(test_params_yml)
    scenario = params.get("arch_systimer_freq", {})
    host_cfg = scenario.get("host", {}) or {}
    serial_wait_ms = int(host_cfg.get("serial_wait_ms", 1000))
    expected_freq_hz = float(host_cfg.get("expected_freq_hz", 1_000_000))
    tolerance_pct = float(host_cfg.get("tolerance_pct", 10.0))
    suite_timeout_s = float(scenario.get("suite_timeout_s", 15.0))

    # Timestamp the arrival of the first pulse marker
    serial.expect(r"arch_systimer_freq:pulse_start", timeout=suite_timeout_s)
    t0 = time.monotonic()

    # Wait for the second marker (arrives after firmware busy-waits)
    serial.expect(r"arch_systimer_freq:pulse_end", timeout=suite_timeout_s)
    t1 = time.monotonic()

    # Compute independent frequency from host wall-clock gap
    measured_interval_s = t1 - t0
    expected_ticks = serial_wait_ms * expected_freq_hz / 1000.0
    measured_freq_hz = expected_ticks / measured_interval_s
    freq_err_pct = (measured_freq_hz - expected_freq_hz) / expected_freq_hz * 100.0

    if abs(freq_err_pct) > tolerance_pct:
        raise AssertionError(
            f"arch_systimer_freq: measured_freq={measured_freq_hz:.0f} Hz "
            f"(expected {expected_freq_hz:.0f} Hz, err {freq_err_pct:+.1f}% "
            f"> tolerance {tolerance_pct}%); "
            f"measured_interval={measured_interval_s * 1000:.0f} ms "
            f"(expected {serial_wait_ms} ms)")

    print(f"[PASS] arch_systimer_freq: measured_freq={measured_freq_hz:.0f} Hz "
          f"(err {freq_err_pct:+.1f}%, interval={measured_interval_s * 1000:.0f} ms)")

    # Remaining suite verdict from firmware trace
    serial.expect_test_summary("arch_systimer_freq", timeout=suite_timeout_s)
