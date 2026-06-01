"""timer_oneshot scenario: verify one-shot timer fires after expected delay.

Firmware configures a one-shot timer, waits for the IRQ, and asserts
the elapsed time matches the configured period.
"""

from vsf_bench import SerialInstrument



def run(serial: SerialInstrument, test_params_yml=None):
    serial.expect_test_summary("timer_oneshot", timeout=10.0)
