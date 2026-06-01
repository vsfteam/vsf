"""gpio_analog_mode suite host harness.

Firmware asserts internally via VSF_TEST_ASSERT; this script waits for the
test framework summary line and asserts all cases passed.

VSF_GPIO_ANALOG: input buffer off, output disabled, FUNCSEL=NULL. The
firmware confirms read() returns 0 with pull-up or pull-down asserted —
no external wiring needed.
"""

from vsf_bench import LogicAnalyzerInstrument, SerialInstrument



def run(serial: SerialInstrument, test_params_yml=None):
    serial.expect_test_summary("gpio_analog_mode")
