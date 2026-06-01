"""timer_periodic scenario: verify periodic timer fires 5 consecutive interrupts.

Firmware configures a periodic timer, counts 5 interrupts in the ISR,
and asserts the counter reaches 5.
"""

from vsf_bench import SerialInstrument



def run(serial: SerialInstrument, test_params_yml=None):
    serial.expect_test_summary("timer_periodic", timeout=10.0)
