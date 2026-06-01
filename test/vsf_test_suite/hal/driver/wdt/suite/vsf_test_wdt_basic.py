"""wdt_basic scenario: verify watchdog can be fed and does not reset.

Firmware feeds the watchdog within the timeout window and asserts
no reset occurs.
"""

from vsf_bench import SerialInstrument



def run(serial: SerialInstrument, test_params_yml=None):
    serial.expect_test_summary("wdt_basic", timeout=10.0)
