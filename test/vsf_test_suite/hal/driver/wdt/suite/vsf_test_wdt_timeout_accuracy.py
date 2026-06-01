"""wdt_timeout_accuracy — verify WDT actual reset time vs configured timeout.

Two-phase test (same pattern as wdt_reboot):
Phase 1 — Firmware saves start time to scratch registers, arms WDT, stops
         feeding → chip resets.
Phase 2 — At boot, vsf_test_hw_setup checks scratch registers and emits
         "WDT:ACCURACY:RESULT expected=N actual=M ms" before the REPL starts.

Host verifies the reported deviation is within tolerance_pct.
"""

import re
from vsf_bench import SerialInstrument


def run(serial: SerialInstrument, test_params_yml=None):
    # Read expected timeout and tolerance from test_params
    params = test_params_yml or {}
    timeout_ms = params.get("timeout_ms", 500)
    tolerance_pct = params.get("tolerance_pct", 10)

    # Phase 1 — wait for firmware to arm the WDT
    serial.expect("WDT:ACCURACY:ARMED", timeout=5.0)

    # Wait for serial disconnect (WDT reset kills the CDC device)
    import time
    print(f"[wdt_timeout_accuracy] Armed (timeout={timeout_ms}ms), waiting for reset...")
    disconnect_deadline = time.monotonic() + 10.0
    while time.monotonic() < disconnect_deadline:
        try:
            serial.read_all(timeout=0.5)
        except (Exception, OSError):
            break

    # Reconnect after reset
    print("[wdt_timeout_accuracy] Reconnecting after reset...")
    serial.reconnect(timeout=15.0)

    # Phase 2 — read the accuracy result emitted at boot
    line = serial.expect("WDT:ACCURACY:RESULT expected=\\d+ actual=\\d+ ms",
                         timeout=10.0)
    if line is None:
        print("[FAIL] wdt_timeout_accuracy: no accuracy result after reset")
        return

    m = re.search(r"expected=(\d+)\s+actual=(\d+)\s+ms", line)
    if not m:
        print(f"[FAIL] wdt_timeout_accuracy: unparseable result: {line}")
        return

    expected_ms = int(m.group(1))
    actual_ms = int(m.group(2))
    deviation_pct = abs((int)(actual_ms) - (int)(expected_ms)) * 100.0 / expected_ms

    print(f"[wdt_timeout_accuracy] expected={expected_ms}ms actual={actual_ms}ms "
          f"deviation={deviation_pct:.1f}%")

    if deviation_pct <= tolerance_pct:
        print(f"[PASS] wdt_timeout_accuracy: deviation {deviation_pct:.1f}% "
              f"<= {tolerance_pct}%")
    else:
        print(f"[FAIL] wdt_timeout_accuracy: deviation {deviation_pct:.1f}% "
              f"> {tolerance_pct}%")
