"""wdt_reboot scenario: verify watchdog triggers system reset.

Two-phase test:
1. Firmware arms WDT, emits WDT:ARMED, then stops feeding → chip resets.
2. Host waits for serial disconnect, then reconnects and expects
   "VSF Test Ready" after the reset.
"""


from vsf_bench import SerialInstrument



def run(serial: SerialInstrument, test_params_yml=None):
    # Wait for WDT:ARMED marker — confirms the firmware armed the watchdog.
    serial.expect("WDT:ARMED", timeout=5.0)

    # Wait for serial disconnect (WDT reset kills the CDC device).
    # Poll until the port disappears or read fails.
    import time
    print("[wdt_reboot] Waiting for WDT reset (serial disconnect)...")
    disconnect_deadline = time.monotonic() + 10.0
    disconnected = False
    while time.monotonic() < disconnect_deadline:
        try:
            serial.read_all(timeout=0.5)
        except (Exception, OSError):
            disconnected = True
            break
        # If no data for a while, assume reset happened
        if not disconnected and time.monotonic() > disconnect_deadline - 2.0:
            disconnected = True
            break

    if not disconnected:
        print("[wdt_reboot] Warning: serial did not disconnect, continuing...")

    # Reconnect after reset
    print("[wdt_reboot] Reconnecting after reset...")
    serial.reconnect(timeout=15.0)

    # Wait for firmware boot banner
    serial.expect("VSF Test Ready", timeout=10.0)
    print("[PASS] wdt_reboot: WDT reset detected and device re-enumerated")
